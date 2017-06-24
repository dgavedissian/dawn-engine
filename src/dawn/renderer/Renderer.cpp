/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Renderer.h"
#include "renderer/api/GLRenderContext.h"

namespace dw {
Memory::Memory(const void *data, uint size) : data_{nullptr}, size_{size} {
    data_ = new byte[size];
    memcpy(data_, data, size);
}

Memory::~Memory() {
    if (data_) {
        delete[] data_;
    }
}

Memory::Memory(Memory &&other) {
    *this = std::move(other);
}

Memory& Memory::operator=(Memory&& other) {
    data_ = other.data_;
    size_ = other.size_;
    other.data_ = nullptr;
    other.size_ = 0;
    return *this;
}

void *Memory::data() const {
    return data_;
}

uint Memory::size() const {
    return size_;
}

RenderContext::RenderContext(Context* context) : Object{context} {
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
    auto handle = vertex_buffer_handle_.next();
    addCommand(cmd::CreateVertexBuffer{handle, Memory{data, size}, decl});
    return handle;
}

void Renderer::setVertexBuffer(VertexBufferHandle handle) {
    addCommand(cmd::SetVertexBuffer{handle});
}

ShaderHandle Renderer::createShader(ShaderType type, const String& source) {
    auto handle = shader_handle_.next();
    addCommand(cmd::CreateShader{handle, type, source});
    return handle;
}

ProgramHandle Renderer::createProgram() {
    auto handle = program_handle_.next();
    addCommand(cmd::CreateProgram{handle});
    return handle;
}

void Renderer::attachShader(ProgramHandle program, ShaderHandle shader) {
    addCommand(cmd::AttachShader{program, shader});
}

void Renderer::linkProgram(ProgramHandle program) {
    addCommand(cmd::LinkProgram{program});
}

void Renderer::clear(const Vec3& colour) {
    addCommand(cmd::Clear{colour});
}

void Renderer::submit(ProgramHandle program, uint vertex_count) {
    addCommand(cmd::Submit{program, vertex_count});
}

void Renderer::addCommand(RenderCommand command) {
    command_buffer_[submit_command_buffer_].emplace_back(std::move(command));
}

void Renderer::renderThread() {
    glfwMakeContextCurrent(window_);

    r_render_context_ = makeUnique<GLRenderContext>(context());

    // Enter render loop.
    while (!should_exit_.load()) {
        // Drain command buffer.
        for (auto& command : command_buffer_[render_command_buffer_]) {
            r_render_context_->processCommand(command);
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