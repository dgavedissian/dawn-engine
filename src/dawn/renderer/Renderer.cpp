/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Renderer.h"
#include "renderer/api/GLRenderContext.h"

namespace dw {
Memory::Memory(const void* data, uint size) : data_{nullptr}, size_{size} {
    data_ = new byte[size];
    memcpy(data_, data, size);
}

Memory::~Memory() {
    if (data_) {
        delete[] data_;
    }
}

Memory::Memory(Memory&& other) noexcept {
    *this = std::move(other);
}

Memory& Memory::operator=(Memory&& other) noexcept {
    data_ = other.data_;
    size_ = other.size_;
    other.data_ = nullptr;
    other.size_ = 0;
    return *this;
}

void* Memory::data() const {
    return data_;
}

uint Memory::size() const {
    return size_;
}

RenderContext::RenderContext(Context* context) : Object{context} {
}

void RenderItem::clear() {
    vb = 0;
    ib = 0;
    primitive_count = 0;
    program = 0;
    uniforms.clear();
    for (auto& texture : textures) {
        texture.handle = 0;
    }
}

Renderer::Renderer(Context* context, Window* window)
    : Object{context},
      window_{window->window_},
      frame_barrier_{2},
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
    render_thread_ = Thread{[this]() { renderThread(); }};
}

Renderer::~Renderer() {
    // Flag to the render thread that it should exit.
    should_exit_.store(true);

    // Wait for the render thread to complete its last frame.
    frame_barrier_.wait();

    // Wait for the render thread to exit completely.
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

    // Wait for render thread.
    frame_barrier_.wait();

    // Wait for frame swap, then reset swapped_frames_. This has no race here, because the render
    // thread will not modify "swapped_frames_" again until after this thread hits the barrier
    // again.
    UniqueLock<Mutex> lock{swap_mutex_};
    swap_cv_.wait(lock, [this] { return swapped_frames_; });
    swapped_frames_ = false;
}

VertexBufferHandle Renderer::createVertexBuffer(const void* data, uint size,
                                                const VertexDecl& decl) {
    // TODO: Validate data.
    auto handle = vertex_buffer_handle_.next();
    submitPreFrameCommand(cmd::CreateVertexBuffer{handle, Memory{data, size}, decl});
    return handle;
}

void Renderer::setVertexBuffer(VertexBufferHandle handle) {
    submit_->current_item.vb = handle;
}

void Renderer::deleteVertexBuffer(VertexBufferHandle handle) {
    submitPostFrameCommand(cmd::DeleteVertexBuffer{handle});
}

IndexBufferHandle Renderer::createIndexBuffer(const void* data, uint size, IndexBufferType type) {
    auto handle = index_buffer_handle_.next();
    submitPreFrameCommand(cmd::CreateIndexBuffer{handle, Memory{data, size}, type});
    return handle;
}

void Renderer::setIndexBuffer(IndexBufferHandle handle) {
    submit_->current_item.ib = handle;
}

void Renderer::deleteIndexBuffer(IndexBufferHandle handle) {
    submitPostFrameCommand(cmd::DeleteIndexBuffer{handle});
}

ShaderHandle Renderer::createShader(ShaderType type, const String& source) {
    auto handle = shader_handle_.next();
    submitPreFrameCommand(cmd::CreateShader{handle, type, source});
    return handle;
}

void Renderer::deleteShader(ShaderHandle handle) {
    submitPostFrameCommand(cmd::DeleteShader{handle});
}

ProgramHandle Renderer::createProgram() {
    auto handle = program_handle_.next();
    submitPreFrameCommand(cmd::CreateProgram{handle});
    return handle;
}

void Renderer::attachShader(ProgramHandle program, ShaderHandle shader) {
    submitPreFrameCommand(cmd::AttachShader{program, shader});
}

void Renderer::linkProgram(ProgramHandle program) {
    submitPreFrameCommand(cmd::LinkProgram{program});
}

void Renderer::deleteProgram(ProgramHandle program) {
    submitPostFrameCommand(cmd::DeleteProgram{program});
}

void Renderer::setUniform(const String& uniform_name, int value) {
    submit_->current_item.uniforms[uniform_name] = value;
}

void Renderer::setUniform(const String& uniform_name, float value) {
    submit_->current_item.uniforms[uniform_name] = value;
}

void Renderer::setUniform(const String& uniform_name, const Vec2& value) {
    submit_->current_item.uniforms[uniform_name] = value;
}

void Renderer::setUniform(const String& uniform_name, const Vec3& value) {
    submit_->current_item.uniforms[uniform_name] = value;
}

void Renderer::setUniform(const String& uniform_name, const Vec4& value) {
    submit_->current_item.uniforms[uniform_name] = value;
}

void Renderer::setUniform(const String& uniform_name, const Mat3& value) {
    submit_->current_item.uniforms[uniform_name] = value;
}

void Renderer::setUniform(const String& uniform_name, const Mat4& value) {
    submit_->current_item.uniforms[uniform_name] = value;
}

TextureHandle Renderer::createTexture2D(u16 width, u16 height, TextureFormat format,
                                        const void* data, u32 size) {
    auto handle = texture_handle_.next();
    submitPreFrameCommand(cmd::CreateTexture2D{handle, width, height, format, Memory{data, size}});
    return handle;
}

void Renderer::setTexture(TextureHandle handle, uint sampler_unit) {
    // TODO: check precondition: texture_unit < MAX_TEXTURE_UNITS
    submit_->current_item.textures[sampler_unit].handle = handle;
}

void Renderer::deleteTexture(TextureHandle handle) {
    submitPostFrameCommand(cmd::DeleteTexture{handle});
}

void Renderer::clear(const Vec3& colour) {
    // addCommand(cmd::Clear{colour});
}

void Renderer::submit(ProgramHandle program) {
    submit_->current_item.program = program;
    submit_->render_items.emplace_back(submit_->current_item);
    submit_->current_item.clear();
}

void Renderer::submit(ProgramHandle program, uint vertex_count) {
    submit_->current_item.program = program;
    submit_->current_item.primitive_count = vertex_count / 3;
    submit_->render_items.emplace_back(submit_->current_item);
    submit_->current_item.clear();
}

void Renderer::submitPreFrameCommand(RenderCommand command) {
    submit_->commands_pre.emplace_back(std::move(command));
}

void Renderer::submitPostFrameCommand(RenderCommand command) {
    submit_->commands_post.emplace_back(std::move(command));
}

void Renderer::renderThread() {
    glfwMakeContextCurrent(window_);

    r_render_context_ = makeUnique<GLRenderContext>(context());

    // Enter render loop.
    while (!should_exit_.load()) {
        // Drain command buffers.
        r_render_context_->processCommandList(render_->commands_pre);
        r_render_context_->submit(render_->render_items);
        r_render_context_->processCommandList(render_->commands_post);
        render_->current_item.clear();
        render_->render_items.clear();
        render_->commands_pre.clear();
        render_->commands_post.clear();

        // Swap buffers.
        glfwSwapBuffers(window_);

        // Wait for submit thread.
        frame_barrier_.wait();

        // Swap command buffers and unblock submit thread.
        {
            std::lock_guard<std::mutex> lock{swap_mutex_};
            std::swap(submit_, render_);
            swapped_frames_ = true;
        }
        swap_cv_.notify_all();
    }
}

}  // namespace dw