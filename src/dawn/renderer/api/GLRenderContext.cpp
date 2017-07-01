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

void GLRenderContext::processCommandList(Vector<RenderCommand>& command_list) {
    for (auto& command : command_list) {
        mpark::visit(*this, command);
    }
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

void GLRenderContext::operator()(const cmd::DeleteVertexBuffer& c) {
    // TODO: implement.
}

void GLRenderContext::operator()(const cmd::CreateIndexBuffer& c) {
    // Create element buffer object.
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, c.data.size(), c.data.data(), GL_STATIC_DRAW);
    CHECK();

    r_index_buffer_map_.emplace(
        c.handle,
        makePair(ebo, c.type == IndexBufferType::U16 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT));
}

void GLRenderContext::operator()(const cmd::DeleteIndexBuffer& c) {
    // TODO: implement.
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

void GLRenderContext::operator()(const cmd::DeleteShader& c) {
    auto it = r_shader_map_.find(c.handle);
    glDeleteShader(it->second);
    r_shader_map_.erase(it);
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

void GLRenderContext::operator()(const cmd::DeleteProgram& c) {
    auto it = r_program_map_.find(c.handle);
    glDeleteProgram(it->second);
    r_program_map_.erase(it);
}

void GLRenderContext::operator()(const cmd::CreateTexture2D& c) {
}

void GLRenderContext::operator()(const cmd::DeleteTexture& c) {
    auto it = r_texture_map_.find(c.handle);
    glDeleteTextures(1, &it->second);
    r_texture_map_.erase(it);
}

void GLRenderContext::submit(const Vector<RenderItem>& items) {
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    for (uint i = 0; i < items.size(); ++i) {
        auto* previous = i > 0 ? &items[i - 1] : nullptr;
        auto* current = &items[i];

        // Bind VAO.
        if (!previous || previous->vb != current->vb) {
            assert(current->vb != 0);
            glBindVertexArray(r_vertex_buffer_map_[current->vb]);
        }

        // Bind EBO.
        if (!previous || previous->ib != current->ib) {
            if (current->ib != 0) {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r_index_buffer_map_[current->ib].second);
            } else {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }
        }

        // Bind Program.
        if (!previous || previous->program != current->program) {
            assert(current->program != 0);
            glUseProgram(r_program_map_[current->program]);
        }

        // Submit.
        if (current->ib != 0) {
            glDrawElements(GL_TRIANGLES, current->primitive_count * 3,
                           r_index_buffer_map_[current->ib].second, 0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, current->primitive_count * 3);
        }
    }
}

}  // namespace dw