/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Renderer.h"

namespace dw {
Renderer::Renderer(Context* context, Window* window)
    : Object{context},
      window_{window->window_},
      submit_command_buffer_{0},
      render_command_buffer_{1} {
    // Attach GL context to main thread.
    glfwMakeContextCurrent(window_);

    // Initialise GL extensions.
    if (gl3wInit() != 0) {
        throw Exception{"gl3wInit failed."};
    }

    // Detach GL context from main thread.
    glfwMakeContextCurrent(nullptr);

    // Spawn render thread.
    should_exit_.store(false);
    render_thread_ = Thread{[this]() {
        render_lock_.lock();
        renderThread();
    }};

    // Acquire submit lock.
    submit_lock_.lock();
}

Renderer::~Renderer() {
    should_exit_.store(true);
    submit_lock_.unlock();  // Allow the render thread to exit.
    render_thread_.join();
}

void Renderer::pushRenderTask(RenderTask&& task) {
    // render_tasks_.emplace_back(task);
}

void Renderer::frame() {
    /*
    bgfx::setViewRect(0, 0, 0, width_, height_);
    for (auto task : render_tasks_) {
        // Set camera state.
        if (task.type == RenderTaskType::SetCameraMatrices) {
            auto& task_data = task.camera;
            task_data.view_matrix.Transpose();
            task_data.proj_matrix.Transpose();
            bgfx::setViewTransform(0, &task_data.view_matrix.v[0][0],
                                   &task_data.proj_matrix.v[0][0]);
        }
        // Render.
        if (task.type == RenderTaskType::Primitive) {
            auto& task_data = task.primitive;
            task_data.model_matrix.Transpose();
            bgfx::setTransform(&task_data.model_matrix.v[0][0]);
            bgfx::setVertexBuffer(task_data.vb);
            bgfx::setIndexBuffer(task_data.ib);
            bgfx::submit(0, task_data.shader);
        }
    }
    render_tasks_.clear();
    bgfx::frame();
     */

    // Synchronise with rendering thread.
    submit_lock_.unlock();  // Signal to render thread that we are done with cmd buffer.
    render_lock_.lock();    // Block until buffer swap is complete.
    render_lock_.unlock();  // Allow render thread to continue once signalled that swap is done.
    submit_lock_.lock();    // Reacquire lock to submit cmd buffer so render thread waits for us.
}

VertexBufferHandle Renderer::createVertexBuffer(const void* data, uint size,
                                                const VertexDecl& decl) {
    auto& command = addCommand(RenderCommand::Type::CreateVertexBuffer);
    byte* copied_data = new byte[size];
    memcpy(copied_data, data, size);
    command.create_vertex_buffer = {vertex_buffer_handle_.next(), copied_data, size,
                                    new VertexDecl{decl}};
    return command.create_vertex_buffer.handle;
}

void Renderer::setVertexBuffer(VertexBufferHandle handle) {
    auto& command = addCommand(RenderCommand::Type::SetVertexBuffer);
    command.set_vertex_buffer = {handle};
}

ShaderHandle Renderer::createShader(ShaderType type, const String& source) {
    auto& command = addCommand(RenderCommand::Type::CreateShader);
    char* source_data = new char[source.size() + 1];
    memcpy(source_data, source.c_str(), source.size() + 1);
    command.create_shader = {shader_handle_.next(), type, source_data};
    return command.create_shader.handle;
}

ProgramHandle Renderer::createProgram() {
    auto& command = addCommand(RenderCommand::Type::CreateProgram);
    command.create_program = {program_handle_.next()};
    return command.create_program.handle;
}

void Renderer::attachShader(ProgramHandle program, ShaderHandle shader) {
    auto& command = addCommand(RenderCommand::Type::AttachShader);
    command.attach_shader = {program, shader};
}

void Renderer::linkProgram(ProgramHandle program) {
    auto& command = addCommand(RenderCommand::Type::LinkProgram);
    command.link_program = {program};
}

void Renderer::submit(ProgramHandle program, uint vertex_count) {
    auto& command = addCommand(RenderCommand::Type::Submit);
    command.submit = {program, vertex_count};
}

RenderCommand& Renderer::addCommand(RenderCommand::Type type) {
    command_buffer_[submit_command_buffer_].emplace_back();
    auto& command = command_buffer_[submit_command_buffer_].back();
    command.type = type;
    return command;
}

void Renderer::renderThread() {
    glfwMakeContextCurrent(window_);

    log().info("[Renderer] OpenGL: %s - GLSL: %s", glGetString(GL_VERSION),
               glGetString(GL_SHADING_LANGUAGE_VERSION));
    log().info("[Renderer] OpenGL Renderer: %s", glGetString(GL_RENDERER));

    // Enter render loop.
    while (!should_exit_.load()) {
        glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Drain command buffer.
        for (auto& command : command_buffer_[render_command_buffer_]) {
            processCommand(command);
        }
        command_buffer_[render_command_buffer_].clear();

        // Swap buffers.
        glfwSwapBuffers(window_);

        // Wait for submit thread.
        submit_lock_.lock();

        // Swap command buffers.
        std::swap(submit_command_buffer_, render_command_buffer_);

        // Unblock submit thread.
        submit_lock_.unlock();  // Allow submit thread to reacquire it's lock.
        render_lock_.unlock();  // Unblock submit thread by signalling that swap is complete.
        render_lock_.lock();    // Reacquire.
    }
}

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

void Renderer::processCommand(RenderCommand& command) {
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
        case RenderCommand::Type::Submit: {
            auto& c = command.submit;
            glUseProgram(r_program_map_[c.handle]);
            glDrawArrays(GL_TRIANGLES, 0, c.vertex_count);
            break;
        }
    }
}

}  // namespace dw