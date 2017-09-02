/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Renderer.h"
#include "renderer/api/GLRenderContext.h"
#include "renderer/GLSL.h"

namespace dw {
Memory::Memory() : Memory{nullptr, 0} {
}

Memory::Memory(const void* data, uint size) : data_{nullptr}, size_{size} {
    if (data != nullptr) {
        data_ = new byte[size];
        memcpy(data_, data, size);
    }
}

Memory::~Memory() {
    if (data_) {
        delete[] data_;
    }
}

Memory::Memory(const Memory& other) noexcept {
    *this = other;
}

Memory& Memory::operator=(const Memory& other) noexcept {
    if (other.data_ != nullptr) {
        data_ = new byte[other.size_];
        size_ = other.size_;
        memcpy(data_, other.data_, other.size_);
    } else {
        data_ = nullptr;
        size_ = 0;
    }
    return *this;
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

RenderItem::RenderItem() {
    clear();
}

void RenderItem::clear() {
    vb = VertexBufferHandle::invalid;
    ib = IndexBufferHandle::invalid;
    primitive_count = 0;
    primitive_offset = 0;
    program = ProgramHandle::invalid;
    uniforms.clear();
    for (auto& texture : textures) {
        texture.handle = TextureHandle::invalid;
    }

    // Default scissor.
    scissor_enabled = false;
    scissor_x = 0;
    scissor_y = 0;
    scissor_width = 0;
    scissor_height = 0;

    // Default render state.
    cull_face_enabled = true;
    cull_front_face = CullFrontFace::CCW;
    depth_enabled = true;
    blend_enabled = false;
    blend_equation_rgb = blend_equation_a = BlendEquation::Add;
    blend_src_rgb = blend_src_a = BlendFunc::One;
    blend_dest_rgb = blend_dest_a = BlendFunc::Zero;
}

Renderer::Renderer(Context* context)
    : Object{context},
      shared_rt_should_exit_{false},
      shared_rt_finished_{false},
      shared_frame_barrier_{2},
      submit_{&frames_[0]},
      render_{&frames_[1]} {
    glslang::InitializeProcess();
}

Renderer::~Renderer() {
    glslang::FinalizeProcess();
    // Wait for render thread if multithreaded.
    if (use_render_thread_) {
        if (!shared_rt_finished_) {
            // Flag to the render thread that it should exit.
            shared_rt_should_exit_ = true;

            // Wait for the render thread to complete its last frame.
            shared_frame_barrier_.wait();

            // Wait for the render thread to exit completely.
            render_thread_.join();
        }
    }
}

void Renderer::init(u16 width, u16 height, const String& title, bool use_render_thread) {
    width_ = width;
    height_ = height;
    window_title_ = title;
    use_render_thread_ = use_render_thread;

    // Kick off rendering thread.
    shared_render_context_ = makeUnique<GLRenderContext>(context());
    shared_render_context_->createWindow(width_, height_, window_title_);
    if (use_render_thread) {
        render_thread_ = Thread{[this]() { renderThread(); }};
    } else {
        shared_render_context_->startRendering();
    }
}

VertexBufferHandle Renderer::createVertexBuffer(const void* data, uint size, const VertexDecl& decl,
                                                BufferUsage usage) {
    // TODO: Validate data.
    auto handle = vertex_buffer_handle_.next();
    submitPreFrameCommand(cmd::CreateVertexBuffer{handle, Memory{data, size}, decl, usage});
    return handle;
}

void Renderer::setVertexBuffer(VertexBufferHandle handle) {
    submit_->current_item.vb = handle;
}

void Renderer::updateVertexBuffer(VertexBufferHandle handle, const void* data, uint size,
                                  uint offset) {
    // TODO: Validate data.
    submitPreFrameCommand(cmd::UpdateVertexBuffer{handle, Memory{data, size}, offset});
}

void Renderer::deleteVertexBuffer(VertexBufferHandle handle) {
    submitPostFrameCommand(cmd::DeleteVertexBuffer{handle});
}

IndexBufferHandle Renderer::createIndexBuffer(const void* data, uint size, IndexBufferType type,
                                              BufferUsage usage) {
    auto handle = index_buffer_handle_.next();
    submitPreFrameCommand(cmd::CreateIndexBuffer{handle, Memory{data, size}, type, usage});
    return handle;
}

void Renderer::setIndexBuffer(IndexBufferHandle handle) {
    submit_->current_item.ib = handle;
}

void Renderer::updateIndexBuffer(IndexBufferHandle handle, const void* data, uint size,
                                 uint offset) {
    // TODO: Validate data.
    submitPreFrameCommand(cmd::UpdateIndexBuffer{handle, Memory{data, size}, offset});
}

void Renderer::deleteIndexBuffer(IndexBufferHandle handle) {
    submitPostFrameCommand(cmd::DeleteIndexBuffer{handle});
}

ShaderHandle Renderer::createShader(ShaderStage stage, const void* data, u32 size) {
    auto handle = shader_handle_.next();
    submitPreFrameCommand(cmd::CreateShader{handle, stage, Memory{data, size}});
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
    setUniform(uniform_name, UniformData{value});
}

void Renderer::setUniform(const String& uniform_name, float value) {
    setUniform(uniform_name, UniformData{value});
}

void Renderer::setUniform(const String& uniform_name, const Vec2& value) {
    setUniform(uniform_name, UniformData{value});
}

void Renderer::setUniform(const String& uniform_name, const Vec3& value) {
    setUniform(uniform_name, UniformData{value});
}

void Renderer::setUniform(const String& uniform_name, const Vec4& value) {
    setUniform(uniform_name, UniformData{value});
}

void Renderer::setUniform(const String& uniform_name, const Mat3& value) {
    setUniform(uniform_name, UniformData{value});
}

void Renderer::setUniform(const String& uniform_name, const Mat4& value) {
    setUniform(uniform_name, UniformData{value});
}

void Renderer::setUniform(const String& uniform_name, UniformData data) {
    submit_->current_item.uniforms[uniform_name] = data;
}

TextureHandle Renderer::createTexture2D(u16 width, u16 height, TextureFormat format,
                                        const void* data, u32 size) {
    auto handle = texture_handle_.next();
    texture_data_[handle] = {width, height, format};
    submitPreFrameCommand(cmd::CreateTexture2D{handle, width, height, format, Memory{data, size}});
    return handle;
}

void Renderer::setTexture(TextureHandle handle, uint sampler_unit) {
    // TODO: check precondition: texture_unit < MAX_TEXTURE_UNITS
    submit_->current_item.textures[sampler_unit].handle = handle;
}

void Renderer::deleteTexture(TextureHandle handle) {
    texture_data_.erase(handle);
    submitPostFrameCommand(cmd::DeleteTexture{handle});
}

FrameBufferHandle Renderer::createFrameBuffer(u16 width, u16 height, TextureFormat format) {
    auto handle = frame_buffer_handle_.next();
    auto texture_handle = createTexture2D(width, height, format, nullptr, 0);
    frame_buffer_textures_[handle] = {texture_handle};
    submitPreFrameCommand(cmd::CreateFrameBuffer{handle, width, height, {texture_handle}});
    return handle;
}

FrameBufferHandle Renderer::createFrameBuffer(Vector<TextureHandle> textures) {
    auto handle = frame_buffer_handle_.next();
    u16 width = texture_data_.at(textures[0]).width, height = texture_data_.at(textures[0]).height;
    for (size_t i = 1; i < textures.size(); ++i) {
        auto& data = texture_data_.at(textures[i]);
        if (data.width != width || data.height != height) {
            // TODO: error.
            log().error("Frame buffer mismatch at index %d: Expected: %d x %d, Actual: %d x %d", i,
                        width, height, data.width, data.height);
        }
    }
    frame_buffer_textures_[handle] = textures;
    submitPreFrameCommand(cmd::CreateFrameBuffer{handle, width, height, textures});
    return handle;
}

TextureHandle Renderer::getFrameBufferTexture(FrameBufferHandle handle, uint index) {
    auto& textures = frame_buffer_textures_.at(handle);
    return textures[index];
}

void Renderer::deleteFrameBuffer(FrameBufferHandle handle) {
    frame_buffer_textures_.erase(handle);
    submitPostFrameCommand(cmd::DeleteFrameBuffer{handle});
}

void Renderer::setViewClear(uint view, const Colour& colour) {
    submit_->view(view).clear_colour = colour;
}

void Renderer::setViewFrameBuffer(uint view, FrameBufferHandle handle) {
    submit_->view(view).frame_buffer = handle;
}

void Renderer::setStateEnable(RenderState state) {
    switch (state) {
        case RenderState::CullFace:
            submit_->current_item.cull_face_enabled = true;
            break;
        case RenderState::Depth:
            submit_->current_item.depth_enabled = true;
            break;
        case RenderState::Blending:
            submit_->current_item.blend_enabled = true;
            break;
    }
}

void Renderer::setStateDisable(RenderState state) {
    switch (state) {
        case RenderState::CullFace:
            submit_->current_item.cull_face_enabled = false;
            break;
        case RenderState::Depth:
            submit_->current_item.depth_enabled = false;
            break;
        case RenderState::Blending:
            submit_->current_item.blend_enabled = false;
            break;
    }
}

void Renderer::setStateCullFrontFace(CullFrontFace front_face) {
    submit_->current_item.cull_front_face = front_face;
}

void Renderer::setStateBlendEquation(BlendEquation equation, BlendFunc src, BlendFunc dest) {
    setStateBlendEquation(equation, src, dest, equation, src, dest);
}

void Renderer::setStateBlendEquation(BlendEquation equation_rgb, BlendFunc src_rgb,
                                     BlendFunc dest_rgb, BlendEquation equation_a, BlendFunc src_a,
                                     BlendFunc dest_a) {
    submit_->current_item.blend_equation_rgb = equation_rgb;
    submit_->current_item.blend_src_rgb = src_rgb;
    submit_->current_item.blend_dest_rgb = dest_rgb;
    submit_->current_item.blend_equation_a = equation_a;
    submit_->current_item.blend_src_a = src_a;
    submit_->current_item.blend_dest_a = dest_a;
}

void Renderer::setScissor(u16 x, u16 y, u16 width, u16 height) {
    submit_->current_item.scissor_enabled = true;
    submit_->current_item.scissor_x = x;
    submit_->current_item.scissor_y = y;
    submit_->current_item.scissor_width = width;
    submit_->current_item.scissor_height = height;
}

void Renderer::submit(uint view, ProgramHandle program) {
    submit(view, program, 0);
}

void Renderer::submit(uint view, ProgramHandle program, uint vertex_count) {
    submit(view, program, vertex_count, 0);
}

void Renderer::submit(uint view, ProgramHandle program, uint vertex_count, uint offset) {
    submit_->current_item.program = program;
    submit_->current_item.primitive_count = vertex_count / 3;
    submit_->current_item.primitive_offset = offset;
    submit_->view(view).render_items.emplace_back(submit_->current_item);
    submit_->current_item.clear();
}

void Renderer::frame() {
    // If we are rendering in multithreaded mode, wait for the render thread.
    if (use_render_thread_) {
        // If the rendering thread is doing nothing, print a warning and give up.
        if (shared_rt_finished_) {
            log().warn("Rendering thread has finished running. Sending shutdown signal.");
            subsystem<EventSystem>()->triggerEvent(makeShared<EvtData_Exit>());
            return;
        }

        // Wait for render thread.
        shared_frame_barrier_.wait();

        // Wait for frame swap, then reset swapped_frames_. This has no race here, because the
        // render thread will not modify "swapped_frames_" again until after this thread hits the
        // barrier again.
        UniqueLock<Mutex> lock{swap_mutex_};
        swap_cv_.wait(lock, [this] { return swapped_frames_; });
        swapped_frames_ = false;
    } else {
        if (!renderFrame(submit_)) {
            subsystem<EventSystem>()->triggerEvent(makeShared<EvtData_Exit>());
            log().warn("Rendering failed. Sending shutdown signal.");
            return;
        }
    }

    // Update window events.
    shared_render_context_->processEvents();
    if (shared_render_context_->isWindowClosed()) {
        log().info("Window closed. Sending shutdown signal.");
        subsystem<EventSystem>()->triggerEvent(makeShared<EvtData_Exit>());
    }
}

void Renderer::submitPreFrameCommand(RenderCommand&& command) {
    submit_->commands_pre.emplace_back(std::move(command));
}

void Renderer::submitPostFrameCommand(RenderCommand&& command) {
    submit_->commands_post.emplace_back(std::move(command));
}

void Renderer::renderThread() {
    // Start rendering.
    shared_render_context_->startRendering();

    // Start render loop.
    while (!shared_rt_should_exit_) {
        if (!renderFrame(render_)) {
            shared_rt_should_exit_ = true;
        }

        // Wait for submit thread.
        shared_frame_barrier_.wait();

        // Swap command buffers and unblock submit thread.
        {
            std::lock_guard<std::mutex> lock{swap_mutex_};
            std::swap(submit_, render_);
            swapped_frames_ = true;

            // If the render loop has been marked for termination, update the "RT finished" marker.
            if (shared_rt_should_exit_) {
                shared_rt_finished_ = true;
            }
        }
        swap_cv_.notify_all();
    }
}

bool Renderer::renderFrame(Frame* frame) {
    // Hand off commands to the render context.
    shared_render_context_->processCommandList(frame->commands_pre);
    if (!shared_render_context_->frame(frame->views)) {
        return false;
    }
    shared_render_context_->processCommandList(frame->commands_post);

    // Clear the frame state.
    frame->current_item.clear();
    for (auto& view : frame->views) {
        view.render_items.clear();
    }
    frame->commands_pre.clear();
    frame->commands_post.clear();

    return true;
}
}  // namespace dw
