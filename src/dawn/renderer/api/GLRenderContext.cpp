/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/api/GLRenderContext.h"

#define GL_CHECK(X) __CHECK(X, __FILE__, __LINE__)
#define __CHECK(X, FILE, LINE)                                    \
    X;                                                            \
    {                                                             \
        GLuint err = glGetError();                                \
        if (err != 0) {                                           \
            log().error("glGetError() returned 0x%.4X", err);     \
            log().error("Function: %s in %s:%s", #X, FILE, LINE); \
            assert(false);                                        \
        }                                                         \
    }

namespace dw {
namespace {
// GL TextureFormatInfo.
struct TextureFormatInfo {
    GLenum internal_format;
    GLenum internal_format_srgb;
    GLenum format;
    GLenum type;
    bool supported;
};

// clang-format off
TextureFormatInfo s_texture_format[] =
    {
        { GL_ALPHA,              GL_ZERO,         GL_ALPHA,            GL_UNSIGNED_BYTE,                false }, // A8
        { GL_R8,                 GL_ZERO,         GL_RED,              GL_UNSIGNED_BYTE,                false }, // R8
        { GL_R8I,                GL_ZERO,         GL_RED,              GL_BYTE,                         false }, // R8I
        { GL_R8UI,               GL_ZERO,         GL_RED,              GL_UNSIGNED_BYTE,                false }, // R8U
        { GL_R8_SNORM,           GL_ZERO,         GL_RED,              GL_BYTE,                         false }, // R8S
        { GL_R16,                GL_ZERO,         GL_RED,              GL_UNSIGNED_SHORT,               false }, // R16
        { GL_R16I,               GL_ZERO,         GL_RED,              GL_SHORT,                        false }, // R16I
        { GL_R16UI,              GL_ZERO,         GL_RED,              GL_UNSIGNED_SHORT,               false }, // R16U
        { GL_R16F,               GL_ZERO,         GL_RED,              GL_HALF_FLOAT,                   false }, // R16F
        { GL_R16_SNORM,          GL_ZERO,         GL_RED,              GL_SHORT,                        false }, // R16S
        { GL_R32I,               GL_ZERO,         GL_RED,              GL_INT,                          false }, // R32I
        { GL_R32UI,              GL_ZERO,         GL_RED,              GL_UNSIGNED_INT,                 false }, // R32U
        { GL_R32F,               GL_ZERO,         GL_RED,              GL_FLOAT,                        false }, // R32F
        { GL_RG8,                GL_ZERO,         GL_RG,               GL_UNSIGNED_BYTE,                false }, // RG8
        { GL_RG8I,               GL_ZERO,         GL_RG,               GL_BYTE,                         false }, // RG8I
        { GL_RG8UI,              GL_ZERO,         GL_RG,               GL_UNSIGNED_BYTE,                false }, // RG8U
        { GL_RG8_SNORM,          GL_ZERO,         GL_RG,               GL_BYTE,                         false }, // RG8S
        { GL_RG16,               GL_ZERO,         GL_RG,               GL_UNSIGNED_SHORT,               false }, // RG16
        { GL_RG16I,              GL_ZERO,         GL_RG,               GL_SHORT,                        false }, // RG16I
        { GL_RG16UI,             GL_ZERO,         GL_RG,               GL_UNSIGNED_SHORT,               false }, // RG16U
        { GL_RG16F,              GL_ZERO,         GL_RG,               GL_FLOAT,                        false }, // RG16F
        { GL_RG16_SNORM,         GL_ZERO,         GL_RG,               GL_SHORT,                        false }, // RG16S
        { GL_RG32I,              GL_ZERO,         GL_RG,               GL_INT,                          false }, // RG32I
        { GL_RG32UI,             GL_ZERO,         GL_RG,               GL_UNSIGNED_INT,                 false }, // RG32U
        { GL_RG32F,              GL_ZERO,         GL_RG,               GL_FLOAT,                        false }, // RG32F
        { GL_RGB8,               GL_SRGB8,        GL_RGB,              GL_UNSIGNED_BYTE,                false }, // RGB8
        { GL_RGB8I,              GL_ZERO,         GL_RGB,              GL_BYTE,                         false }, // RGB8I
        { GL_RGB8UI,             GL_ZERO,         GL_RGB,              GL_UNSIGNED_BYTE,                false }, // RGB8U
        { GL_RGB8_SNORM,         GL_ZERO,         GL_RGB,              GL_BYTE,                         false }, // RGB8S
        { GL_RGBA8,              GL_SRGB8_ALPHA8, GL_BGRA,             GL_UNSIGNED_BYTE,                false }, // BGRA8
        { GL_RGBA8,              GL_SRGB8_ALPHA8, GL_RGBA,             GL_UNSIGNED_BYTE,                false }, // RGBA8
        { GL_RGBA8I,             GL_ZERO,         GL_RGBA,             GL_BYTE,                         false }, // RGBA8I
        { GL_RGBA8UI,            GL_ZERO,         GL_RGBA,             GL_UNSIGNED_BYTE,                false }, // RGBA8U
        { GL_RGBA8_SNORM,        GL_ZERO,         GL_RGBA,             GL_BYTE,                         false }, // RGBA8S
        { GL_RGBA16,             GL_ZERO,         GL_RGBA,             GL_UNSIGNED_SHORT,               false }, // RGBA16
        { GL_RGBA16I,            GL_ZERO,         GL_RGBA,             GL_SHORT,                        false }, // RGBA16I
        { GL_RGBA16UI,           GL_ZERO,         GL_RGBA,             GL_UNSIGNED_SHORT,               false }, // RGBA16U
        { GL_RGBA16F,            GL_ZERO,         GL_RGBA,             GL_HALF_FLOAT,                   false }, // RGBA16F
        { GL_RGBA16_SNORM,       GL_ZERO,         GL_RGBA,             GL_SHORT,                        false }, // RGBA16S
        { GL_RGBA32I,            GL_ZERO,         GL_RGBA,             GL_INT,                          false }, // RGBA32I
        { GL_RGBA32UI,           GL_ZERO,         GL_RGBA,             GL_UNSIGNED_INT,                 false }, // RGBA32U
        { GL_RGBA32F,            GL_ZERO,         GL_RGBA,             GL_FLOAT,                        false }, // RGBA32F
        { GL_RGB565,             GL_ZERO,         GL_RGB,              GL_UNSIGNED_SHORT_5_6_5,         false }, // R5G6B5
        { GL_RGBA4,              GL_ZERO,         GL_RGBA,             GL_UNSIGNED_SHORT_4_4_4_4_REV,   false }, // RGBA4
        { GL_RGB5_A1,            GL_ZERO,         GL_RGBA,             GL_UNSIGNED_SHORT_1_5_5_5_REV,   false }, // RGB5A1
        { GL_RGB10_A2,           GL_ZERO,         GL_RGBA,             GL_UNSIGNED_INT_2_10_10_10_REV,  false }, // RGB10A2
        { GL_R11F_G11F_B10F,     GL_ZERO,         GL_RGB,              GL_UNSIGNED_INT_10F_11F_11F_REV, false }, // RG11B10F
        { GL_DEPTH_COMPONENT16,  GL_ZERO,         GL_DEPTH_COMPONENT,  GL_UNSIGNED_SHORT,               false }, // D16
        { GL_DEPTH_COMPONENT24,  GL_ZERO,         GL_DEPTH_COMPONENT,  GL_UNSIGNED_INT,                 false }, // D24
        { GL_DEPTH24_STENCIL8,   GL_ZERO,         GL_DEPTH_STENCIL,    GL_UNSIGNED_INT_24_8,            false }, // D24S8
        { GL_DEPTH_COMPONENT32,  GL_ZERO,         GL_DEPTH_COMPONENT,  GL_UNSIGNED_INT,                 false }, // D32
        { GL_DEPTH_COMPONENT32F, GL_ZERO,         GL_DEPTH_COMPONENT,  GL_FLOAT,                        false }, // D16F
        { GL_DEPTH_COMPONENT32F, GL_ZERO,         GL_DEPTH_COMPONENT,  GL_FLOAT,                        false }, // D24F
        { GL_DEPTH_COMPONENT32F, GL_ZERO,         GL_DEPTH_COMPONENT,  GL_FLOAT,                        false }, // D32F
        { GL_STENCIL_INDEX8,     GL_ZERO,         GL_STENCIL_INDEX,    GL_UNSIGNED_BYTE,                false }, // D0S8
    };
// clang-format on
static_assert(static_cast<int>(TextureFormat::Count) ==
                  sizeof(s_texture_format) / sizeof(s_texture_format[0]),
              "Texture format mapping mismatch.");
}  // namespace

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
    GL_CHECK();

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

        log().info("[renderer] Attrib %s: Count='%s' Type='%s' Stride='%s' Offset='%s'",
                   attrib_counter, count, (int)gl_type->first, c.decl.stride_,
                   (intptr_t)attrib.second);

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
    GL_CHECK();

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
    ProgramData program_data;
    program_data.program = glCreateProgram();
    r_program_map_.emplace(c.handle, program_data);
}

void GLRenderContext::operator()(const cmd::AttachShader& c) {
    glAttachShader(r_program_map_[c.handle].program, r_shader_map_[c.shader_handle]);
}

void GLRenderContext::operator()(const cmd::LinkProgram& c) {
    GLuint program = r_program_map_[c.handle].program;
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
    glDeleteProgram(it->second.program);
    r_program_map_.erase(it);
}

void GLRenderContext::operator()(const cmd::CreateTexture2D& c) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Give image data to OpenGL.
    TextureFormatInfo format = s_texture_format[static_cast<int>(c.format)];
    log().debug(
        "[renderer] [CreateTexture2D] format %s - internal fmt: 0x%.4X - internal fmt srgb: 0x%.4X "
        "- fmt: 0x%.4X - type: 0x%.4X",
        static_cast<u32>(c.format), format.internal_format, format.internal_format_srgb,
        format.format, format.type);
    glTexImage2D(GL_TEXTURE_2D, 0, format.internal_format, c.width, c.height, 0, format.format,
                 format.type, c.data.data());
    GL_CHECK();

    // Add texture.
    r_texture_map_.emplace(c.handle, texture);
}

void GLRenderContext::operator()(const cmd::DeleteTexture& c) {
    auto it = r_texture_map_.find(c.handle);
    glDeleteTextures(1, &it->second);
    r_texture_map_.erase(it);
}

void GLRenderContext::submit(const Vector<RenderItem>& items) {
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (uint i = 0; i < items.size(); ++i) {
        auto* previous = i > 0 ? &items[i - 1] : nullptr;
        auto* current = &items[i];

        // Bind VAO.
        if (!previous || previous->vb != current->vb) {
            assert(current->vb != 0);
            glBindVertexArray(r_vertex_buffer_map_[current->vb]);
            GL_CHECK();
        }

        // Bind EBO.
        if (!previous || previous->ib != current->ib) {
            if (current->ib != 0) {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r_index_buffer_map_[current->ib].first);
                GL_CHECK();
            } else {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                GL_CHECK();
            }
        }

        // Bind Program.
        if (!previous || previous->program != current->program) {
            assert(current->program != 0);
            ProgramData& program_data = r_program_map_[current->program];
            glUseProgram(program_data.program);
            GL_CHECK();

            // Bind Uniforms.
            for (auto& uniform_pair : current->uniforms) {
                struct UniformBinder {
                    GLint uniform_location;
                    UniformBinder(GLint location) : uniform_location{location} {
                    }

                    void operator()(const int& value) {
                        glUniform1i(uniform_location, value);
                    }

                    void operator()(const float& value) {
                        glUniform1f(uniform_location, value);
                    }

                    void operator()(const Vec2& value) {
                        glUniform2f(uniform_location, value.x, value.y);
                    }

                    void operator()(const Vec3& value) {
                        glUniform3f(uniform_location, value.x, value.y, value.z);
                    }

                    void operator()(const Vec4& value) {
                        glUniform4f(uniform_location, value.x, value.y, value.z, value.w);
                    }

                    void operator()(const Mat3& value) {
                        glUniformMatrix3fv(uniform_location, 1, GL_TRUE, value.ptr());
                    }

                    void operator()(const Mat4& value) {
                        glUniformMatrix4fv(uniform_location, 1, GL_TRUE, value.ptr());
                    }
                };
                auto location = program_data.uniform_location_map.find(uniform_pair.first);
                GLint uniform_location;
                if (location != program_data.uniform_location_map.end()) {
                    uniform_location = (*location).second;
                } else {
                    uniform_location =
                        glGetUniformLocation(program_data.program, uniform_pair.first.c_str());
                    GL_CHECK();
                    program_data.uniform_location_map.emplace(uniform_pair.first, uniform_location);
                    if (uniform_location == -1) {
                        log().warn("Unknown uniform '%s', skipping.", uniform_pair.first);
                    }
                }
                if (uniform_location == -1) {
                    continue;
                }
                mpark::visit(UniformBinder{uniform_location}, uniform_pair.second);
                GL_CHECK();
            }

            // Bind textures.
            for (uint j = 0; j < MAX_TEXTURE_SAMPLERS; j++) {
                if (!previous || previous->textures[j].handle != current->textures[j].handle) {
                    glActiveTexture(GL_TEXTURE0 + j);
                    GL_CHECK();
                    glBindTexture(GL_TEXTURE_2D, current->textures[j].handle);
                    GL_CHECK();
                }
            }
        }

        // Submit.
        if (current->primitive_count > 0) {
            if (current->ib != 0) {
                glDrawElements(GL_TRIANGLES, current->primitive_count * 3,
                               r_index_buffer_map_[current->ib].second, 0);
                GL_CHECK();
            } else {
                glDrawArrays(GL_TRIANGLES, 0, current->primitive_count * 3);
                GL_CHECK();
            }
        }
    }
}

}  // namespace dw