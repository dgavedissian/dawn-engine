/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Renderer.h"
#include "Renderer.h"

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
    render_thread_ = Thread{[this]() { renderThread(); }};
}

Renderer::~Renderer() {
    should_exit_.store(true);
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
}

ShaderHandle Renderer::createShader(ShaderType type, const String& source) {
    ShaderHandle handle = shader_handle_.next();
    RenderCommand command;
    command.type = RenderCommand::Type::CreateShader;
    command.create_shader.handle = handle;
    command.create_shader.type = type;
    command.create_shader.source = source;
    pushCommand(std::move(command));
    return handle;
}

ProgramHandle Renderer::createProgram() {
    return 0;
}

void Renderer::attachShader(ProgramHandle program, ShaderHandle shader) {

}

void Renderer::linkProgram() {

}

void Renderer::submit(ProgramHandle program) {

}

void Renderer::pushCommand(RenderCommand command) {
    command_buffer_[submit_command_buffer_].emplace_back(std::move(command));
}

void Renderer::renderThread() {
    glfwMakeContextCurrent(window_);

    log().info("[Renderer] OpenGL: %s - GLSL: %s", glGetString(GL_VERSION),
               glGetString(GL_SHADING_LANGUAGE_VERSION));
    log().info("[Renderer] OpenGL Renderer: %s", glGetString(GL_RENDERER));

    // Enter render loop.
    while (!should_exit_.load()) {
        glClearColor(0.0f, 0.05f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Drain command buffer.
        // TODO.

        // Swap command buffers.
        std::swap(submit_command_buffer_, render_command_buffer_);
        command_buffer_[submit_command_buffer_].clear();

        // Swap buffers.
        glfwSwapBuffers(window_);
    }
}
}  // namespace dw