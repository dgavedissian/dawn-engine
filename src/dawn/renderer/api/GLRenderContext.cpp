/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/api/GLRenderContext.h"

#define CHECK(X) __CHECK(X, __FILE__, __LINE__)
#define __CHECK(X, FILE, LINE)                                    \
    X;                                                            \
    {                                                             \
        GLuint err = glGetError();                                \
        if (err != 0) {                                           \
            log().error("glGetError() returned %s", err);         \
            log().error("Function: %s in %s:%s", #X, FILE, LINE); \
            exit(EXIT_FAILURE);                                   \
        }                                                         \
    }

namespace dw {
GLRenderContext::GLRenderContext(Context* context) : RenderContext{context} {
    log().info("[Renderer] OpenGL: %s - GLSL: %s", glGetString(GL_VERSION),
               glGetString(GL_SHADING_LANGUAGE_VERSION));
    log().info("[Renderer] OpenGL Renderer: %s", glGetString(GL_RENDERER));
}

GLRenderContext::~GLRenderContext() {
}

void GLRenderContext::processCommand(RenderCommand& command) {
    std::experimental::visit(*this, command);
}

void GLRenderContext::operator()(const cmd::CreateVertexBuffer& c) {
    // Create vertex array object.
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create vertex buffer object.
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, c.data.size(), c.data.data(), GL_STATIC_DRAW);
    CHECK();

    // Set up vertex array attributes.
    static HashMap<VertexDecl::AttributeType, GLenum> attribute_type_map = {
        {VertexDecl::AttributeType::Float, GL_FLOAT},
        {VertexDecl::AttributeType::Uint8, GL_UNSIGNED_BYTE}};
    u16 attrib_counter = 0;
    for (auto& attrib : c.decl.attributes_) {
        // Decode attribute.
        VertexDecl::Attribute attribute;
        uint count;
        VertexDecl::AttributeType type;
        bool normalised;
        VertexDecl::decodeAttributes(attrib.first, attribute, count, type, normalised);

        // Convert type.
        auto gl_type = attribute_type_map.find(type);
        if (gl_type == attribute_type_map.end()) {
            log().warn("[renderer] Unknown attribute type: %i", (uint)type);
            continue;
        }

        log().info("%s %s %s %s %s", attrib_counter, count, (int)gl_type->first, c.decl.stride_,
                   attrib.second);

        // Set attribute.
        glEnableVertexAttribArray(attrib_counter);
        glVertexAttribPointer(attrib_counter, count, gl_type->second,
                              normalised ? GL_TRUE : GL_FALSE, c.decl.stride_, attrib.second);
        attrib_counter++;
    }
    r_vertex_buffer_map_.emplace(c.handle, vao);
}

void GLRenderContext::operator()(const cmd::SetVertexBuffer& c) {
    glBindVertexArray(r_vertex_buffer_map_[c.handle]);
}

void GLRenderContext::operator()(const cmd::CreateShader& c) {
    static HashMap<ShaderType, GLenum> shader_type_map = {
        {ShaderType::Vertex, GL_VERTEX_SHADER},
        {ShaderType::Geometry, GL_GEOMETRY_SHADER},
        {ShaderType::Fragment, GL_FRAGMENT_SHADER}};
    GLuint shader = glCreateShader(shader_type_map[c.type]);
    const char* source[] = {c.source.c_str()};
    glShaderSource(shader, 1, source, nullptr);
    glCompileShader(shader);

    // Check compilation result.
    GLint result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        char* errorMessage = new char[infoLogLength];
        glGetShaderInfoLog(shader, infoLogLength, NULL, errorMessage);
        log().error("Shader Compile Error: %s", errorMessage);
        delete[] errorMessage;

        // TODO: Error
    }

    r_shader_map_.emplace(c.handle, shader);
}

void GLRenderContext::operator()(const cmd::CreateProgram& c) {
    GLuint program = glCreateProgram();
    r_program_map_.emplace(c.handle, program);
}

void GLRenderContext::operator()(const cmd::AttachShader& c) {
    glAttachShader(r_program_map_[c.handle], r_shader_map_[c.shader_handle]);
}

void GLRenderContext::operator()(const cmd::LinkProgram& c) {
    GLuint program = r_program_map_[c.handle];
    glLinkProgram(program);

    // Check the result of the link process.
    GLint result = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (result == GL_FALSE) {
        int infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* errorMessage = new char[infoLogLength];
        glGetProgramInfoLog(program, infoLogLength, NULL, errorMessage);
        log().error("Shader Link Error: %s", errorMessage);
        delete[] errorMessage;
    }
}

void GLRenderContext::operator()(const cmd::Clear& c) {
    glClearColor(c.colour.x, c.colour.y, c.colour.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLRenderContext::operator()(const cmd::Submit& c) {
    glUseProgram(r_program_map_[c.handle]);
    CHECK();
    glDrawArrays(GL_TRIANGLES, 0, c.vertex_count);
    CHECK();
}
}  // namespace dw