/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Renderer.h"
#include "renderer/api/GLRenderer.h"

namespace dw {
RendererAPI::RendererAPI(Context *context) : Object{context} {
}

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

void Renderer::clear(const Vec3& colour) {
    auto& command = addCommand(RenderCommand::Type::Clear);
    command.clear.colour[0] = colour.x;
    command.clear.colour[1] = colour.y;
    command.clear.colour[2] = colour.z;
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

    r_renderer_ = makeUnique<GLRenderer>(context());

    // Enter render loop.
    while (!should_exit_.load()) {
        // Drain command buffer.
        for (auto& command : command_buffer_[render_command_buffer_]) {
            r_renderer_->processCommand(command);
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
}  // namespace dw