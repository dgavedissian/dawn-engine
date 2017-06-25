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

Memory::Memory(Memory &&other) noexcept {
    *this = std::move(other);
}

Memory& Memory::operator=(Memory&& other) noexcept {
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


void RenderItem::clear() {
    vb = 0;
    vertex_count = 0;
    ib = 0;
    index_count = 0;
    program = 0;
    for (auto& texture : textures) {
        texture.handle = 0;
    }
}

Renderer::Renderer(Context* context, Window* window)
    : Object{context},
      window_{window->window_},
      submit_{&frames_[0]},
      render_{&frames_[1]} {
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
    submit_->current_item.vb = handle;
}

void Renderer::deleteVertexBuffer(VertexBufferHandle handle) {
    addCommand(cmd::DeleteVertexBuffer{handle});
}

IndexBufferHandle Renderer::createIndexBuffer(const void *data, uint size, IndexBufferType type) {
    auto handle = index_buffer_handle_.next();
    addCommand(cmd::CreateIndexBuffer{handle, Memory{data, size}, type});
    return handle;
}

void Renderer::setIndexBuffer(IndexBufferHandle handle) {
    submit_->current_item.ib = handle;
}

void Renderer::deleteIndexBuffer(IndexBufferHandle handle) {
    addCommand(cmd::DeleteIndexBuffer{handle});
}

ShaderHandle Renderer::createShader(ShaderType type, const String& source) {
    auto handle = shader_handle_.next();
    addCommand(cmd::CreateShader{handle, type, source});
    return handle;
}

void Renderer::deleteShader(ShaderHandle handle) {
addCommand(cmd::DeleteShader{handle});
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

void Renderer::deleteProgram(ProgramHandle program) {
addCommand(cmd::DeleteProgram{program});
}

TextureHandle Renderer::createTexture2D() {
    auto handle = texture_handle_.next();
    addCommand(cmd::CreateTexture2D{handle});
    return handle;
}

void Renderer::setTexture(TextureHandle handle, uint texture_unit) {
    // TODO: check precondition: texture_unit < MAX_TEXTURE_UNITS
    submit_->current_item.textures[texture_unit].handle = handle;
}

void Renderer::deleteTexture(TextureHandle handle) {
    addCommand(cmd::DeleteTexture{handle});
}

void Renderer::clear(const Vec3& colour) {
    //addCommand(cmd::Clear{colour});
}

void Renderer::submit(ProgramHandle program, uint vertex_count) {
    submit_->current_item.program = program;
    submit_->current_item.vertex_count = vertex_count;
    submit_->render_items.emplace_back(submit_->current_item);
    submit_->current_item.clear();
}

void Renderer::addCommand(RenderCommand command) {
    submit_->commands.emplace_back(std::move(command));
}

void Renderer::renderThread() {
    glfwMakeContextCurrent(window_);

    r_render_context_ = makeUnique<GLRenderContext>(context());

    // Enter render loop.
    while (!should_exit_.load()) {
        // Drain command buffer.
        for (auto& command : render_->commands) {
            r_render_context_->processCommand(command);
        }
        r_render_context_->submit(render_->render_items);
        render_->current_item.clear();
        render_->render_items.clear();
        render_->commands.clear();

        // Swap buffers.
        glfwSwapBuffers(window_);

        // Wait for submit thread.
        submit_lock_.lock();

        // Swap command buffers.
        std::swap(submit_, render_);

        // Unblock submit thread.
        submit_lock_.unlock();  // Allow submit thread to reacquire it's lock.
        render_lock_.unlock();  // Unblock submit thread by signalling that swap is complete.
        render_lock_.lock();    // Reacquire.
    }
}

}  // namespace dw