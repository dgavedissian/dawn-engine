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
    switch (command.type) {
        case RenderCommand::Type::CreateVertexBuffer: {
            auto& c = command.create_vertex_buffer;

            // Create vertex array object.
            GLuint vao;
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            // Create vertex buffer object.
            GLuint vbo;
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, c.size, c.data, GL_STATIC_DRAW);
            delete[] c.data;

            // Set up vertex array attributes.
            static HashMap<VertexDecl::AttributeType, GLenum> attribute_type_map = {
                {VertexDecl::AttributeType::Float, GL_FLOAT},
                {VertexDecl::AttributeType::Uint8, GL_UNSIGNED_BYTE}};
            u16 attrib_counter = 0;
            for (auto& attrib : c.decl->attributes_) {
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

                log().info("%s %s %s %s %s", attrib_counter, count, (int)gl_type->first,
                           c.decl->stride_, attrib.second);

                // Set attribute.
                glEnableVertexAttribArray(attrib_counter);
                CHECK(glVertexAttribPointer(attrib_counter, count, gl_type->second,
                                            normalised ? GL_TRUE : GL_FALSE, c.decl->stride_,
                                            attrib.second));
                attrib_counter++;
            }
            delete c.decl;
            r_vertex_buffer_map_.emplace(c.handle, vao);
            break;
        }
        case RenderCommand::Type::SetVertexBuffer: {
            auto& c = command.set_vertex_buffer;
            glBindVertexArray(r_vertex_buffer_map_[c.handle]);
            break;
        }
        case RenderCommand::Type::CreateShader: {
            auto& c = command.create_shader;
            static HashMap<ShaderType, GLenum> shader_type_map = {
                {ShaderType::Vertex, GL_VERTEX_SHADER},
                {ShaderType::Geometry, GL_GEOMETRY_SHADER},
                {ShaderType::Fragment, GL_FRAGMENT_SHADER}};
            GLuint shader = glCreateShader(shader_type_map[c.type]);
            glShaderSource(shader, 1, &c.source, nullptr);
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
            delete c.source;
            break;
        }
        case RenderCommand::Type::CreateProgram: {
            auto& c = command.create_program;
            GLuint program = glCreateProgram();
            r_program_map_.emplace(c.handle, program);
            break;
        }
        case RenderCommand::Type::AttachShader: {
            auto& c = command.attach_shader;
            glAttachShader(r_program_map_[c.handle], r_shader_map_[c.shader_handle]);
            break;
        }
        case RenderCommand::Type::LinkProgram: {
            auto& c = command.link_program;
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
            break;
        }
        case RenderCommand::Type::Clear: {
            auto& c = command.clear;
            glClearColor(c.colour[0], c.colour[1], c.colour[2], 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            break;
        }
        case RenderCommand::Type::Submit: {
            auto& c = command.submit;
            glUseProgram(r_program_map_[c.handle]);
            glDrawArrays(GL_TRIANGLES, 0, c.vertex_count);
            break;
        }
    }
}
}  // namespace dw