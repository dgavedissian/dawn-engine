/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "RHIRenderer.h"
#include "renderer/rhi/GLRenderContext.h"
#include "renderer/rhi/NullRenderContext.h"
#include "renderer/GLSL.h"

namespace dw {
namespace rhi {
RenderContext::RenderContext(Context* context) : Object{context} {
}

RenderItem::RenderItem() {
    clear();
}

void RenderItem::clear() {
    vb = VertexBufferHandle::invalid;
    vb_offset = 0;
    vertex_decl_override = VertexDecl();
    ib = IndexBufferHandle::invalid;
    ib_offset = 0;
    primitive_count = 0;
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
    polygon_mode = PolygonMode::Fill;
    depth_enabled = true;
    blend_enabled = false;
    blend_equation_rgb = blend_equation_a = BlendEquation::Add;
    blend_src_rgb = blend_src_a = BlendFunc::One;
    blend_dest_rgb = blend_dest_a = BlendFunc::Zero;
    colour_write = true;
    depth_write = true;
}

View::View() : clear_colour{0.0f, 0.0f, 0.0f, 1.0f}, frame_buffer{FrameBufferHandle::invalid} {
}

void View::clear() {
    clear_colour = {0.0f, 0.0f, 0.0f, 1.0f};
    frame_buffer = FrameBufferHandle::invalid;
    render_items.clear();
}

Frame::Frame() {
    current_item.clear();
    transient_vb_storage.data = new byte[DW_MAX_TRANSIENT_VERTEX_BUFFER_SIZE];
    transient_vb_storage.size = 0;
    transient_ib_storage.data = new byte[DW_MAX_TRANSIENT_INDEX_BUFFER_SIZE];
    transient_ib_storage.size = 0;
    next_transient_vertex_buffer_handle_ = TransientVertexBufferHandle{0};
    next_transient_index_buffer_handle_ = TransientIndexBufferHandle{0};

    // By default, view 0 will point to the backbuffer.
    view(0).frame_buffer = FrameBufferHandle{0};
}

Frame::~Frame() {
    delete transient_ib_storage.data;
    delete transient_vb_storage.data;
}

View& Frame::view(uint view_index) {
    if (views.size() <= view_index) {
        views.resize(view_index + 1);
    }
    return views[view_index];
}

RHIRenderer::RHIRenderer(Context* context)
    : Module(context),
      use_render_thread_(false),
      is_first_frame_(true),
      shared_rt_should_exit_(false),
      shared_rt_finished_(false),
      shared_frame_barrier_(2),
      submit_(&frames_[0]),
      render_(&frames_[1]) {
}

RHIRenderer::~RHIRenderer() {
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

void RHIRenderer::init(RendererType type, u16 width, u16 height, const String& title,
                    bool use_render_thread) {
    if (type == RendererType::Null) {
        use_render_thread = false;
    }

    width_ = width;
    height_ = height;
    window_title_ = title;
    use_render_thread_ = use_render_thread;
    is_first_frame_ = true;

    // Initialise transient vb/ib.
    transient_vb_max_size = DW_MAX_TRANSIENT_VERTEX_BUFFER_SIZE;
    transient_vb =
        createVertexBuffer(nullptr, transient_vb_max_size, VertexDecl{}, BufferUsage::Stream);
    transient_ib_max_size = DW_MAX_TRANSIENT_INDEX_BUFFER_SIZE;
    transient_ib = createIndexBuffer(nullptr, transient_ib_max_size, IndexBufferType::U16,
                                     BufferUsage::Stream);
    frames_[0].transient_vb_storage.handle = frames_[1].transient_vb_storage.handle = transient_vb;
    frames_[0].transient_ib_storage.handle = frames_[1].transient_ib_storage.handle = transient_ib;

    // Kick off rendering thread.
    switch (type) {
        case RendererType::Null:
            log().info("Using Null renderer.");
            shared_render_context_ = makeUnique<NullRenderContext>(context());
            break;
        case RendererType::OpenGL:
            log().info("Using OpenGL renderer.");
            shared_render_context_ = makeUnique<GLRenderContext>(context());
            break;
        case RendererType::D3D12:
            log().error("D3D12 renderer unimplemented. Deferring to OpenGL.");
            shared_render_context_ = makeUnique<GLRenderContext>(context());
            break;
    }
    shared_render_context_->createWindow(width_, height_, window_title_);
    if (use_render_thread) {
        render_thread_ = Thread{[this]() { renderThread(); }};
    } else {
        shared_render_context_->startRendering();
    }
}

VertexBufferHandle RHIRenderer::createVertexBuffer(const void* data, uint size, const VertexDecl& decl,
                                                BufferUsage usage) {
    // TODO: Validate data.
    auto handle = vertex_buffer_handle_.next();
    submitPreFrameCommand(cmd::CreateVertexBuffer{handle, Memory{data, size}, size, decl, usage});
    vertex_buffer_decl_[handle] = decl;
    return handle;
}

void RHIRenderer::setVertexBuffer(VertexBufferHandle handle) {
    submit_->current_item.vb = handle;
    submit_->current_item.vb_offset = 0;
    submit_->current_item.vertex_decl_override = VertexDecl{};
}

void RHIRenderer::updateVertexBuffer(VertexBufferHandle handle, const void* data, uint size,
                                  uint offset) {
    // TODO: Validate data.
    submitPreFrameCommand(cmd::UpdateVertexBuffer{handle, Memory{data, size}, offset});

#ifdef DW_DEBUG
    if (submit_->updated_vertex_buffers.count(handle) != 0) {
        log().warn("Warning: Updating vertex buffer %d which has been updated already this frame.",
                   handle.internal());
    } else {
        submit_->updated_vertex_buffers.insert(handle);
    }
#endif
}

void RHIRenderer::deleteVertexBuffer(VertexBufferHandle handle) {
    submitPostFrameCommand(cmd::DeleteVertexBuffer{handle});
}

IndexBufferHandle RHIRenderer::createIndexBuffer(const void* data, uint size, IndexBufferType type,
                                              BufferUsage usage) {
    auto handle = index_buffer_handle_.next();
    submitPreFrameCommand(cmd::CreateIndexBuffer{handle, Memory{data, size}, size, type, usage});
    index_buffer_types_[handle] = type;
    return handle;
}

void RHIRenderer::setIndexBuffer(IndexBufferHandle handle) {
    submit_->current_item.ib = handle;
    submit_->current_item.ib_offset = 0;
}

void RHIRenderer::updateIndexBuffer(IndexBufferHandle handle, const void* data, uint size,
                                 uint offset) {
    // TODO: Validate data.
    submitPreFrameCommand(cmd::UpdateIndexBuffer{handle, Memory{data, size}, offset});

#ifdef DW_DEBUG
    if (submit_->updated_index_buffers.count(handle) != 0) {
        log().warn("Warning: Updating index buffer %d which has been updated already this frame.",
                   handle.internal());
    } else {
        submit_->updated_index_buffers.insert(handle);
    }
#endif
}

void RHIRenderer::deleteIndexBuffer(IndexBufferHandle handle) {
    submitPostFrameCommand(cmd::DeleteIndexBuffer{handle});
}

TransientVertexBufferHandle RHIRenderer::allocTransientVertexBuffer(uint vertex_count,
                                                                 const VertexDecl& decl) {
    // Check that we have enough space.
    uint size = vertex_count * decl.stride();
    if (size > transient_vb_max_size - submit_->transient_vb_storage.size) {
        return TransientVertexBufferHandle{};
    }

    // Allocate handle.
    auto handle = submit_->next_transient_vertex_buffer_handle_++;
    byte* data = submit_->transient_vb_storage.data + submit_->transient_vb_storage.size;
    submit_->transient_vb_storage.size += size;
    submit_->transient_vertex_buffers_[handle] = {data, size, decl};
    return handle;
}

byte* RHIRenderer::getTransientVertexBufferData(TransientVertexBufferHandle handle) {
    auto it = submit_->transient_vertex_buffers_.find(handle);
    if (it != submit_->transient_vertex_buffers_.end()) {
        return it->second.data;
    }
    return nullptr;
}

void RHIRenderer::setVertexBuffer(TransientVertexBufferHandle handle) {
    Frame::TransientVertexBufferData& tvb = submit_->transient_vertex_buffers_.at(handle);
    submit_->current_item.vb = transient_vb;
    submit_->current_item.vb_offset = (uint)(tvb.data - submit_->transient_vb_storage.data);
    submit_->current_item.vertex_decl_override = tvb.decl;
}

TransientIndexBufferHandle RHIRenderer::allocTransientIndexBuffer(uint index_count) {
    // Check that we have enough space.
    uint size = index_count * sizeof(u16);
    if (size > transient_ib_max_size - submit_->transient_ib_storage.size) {
        return TransientIndexBufferHandle{};
    }

    // Allocate handle.
    auto handle = submit_->next_transient_index_buffer_handle_++;
    byte* data = submit_->transient_ib_storage.data + submit_->transient_ib_storage.size;
    submit_->transient_ib_storage.size += size;
    submit_->transient_index_buffers_[handle] = {data, size};
    return handle;
}

byte* RHIRenderer::getTransientIndexBufferData(TransientIndexBufferHandle handle) {
    auto it = submit_->transient_index_buffers_.find(handle);
    if (it != submit_->transient_index_buffers_.end()) {
        return it->second.data;
    }
    return nullptr;
}

void RHIRenderer::setIndexBuffer(TransientIndexBufferHandle handle) {
    Frame::TransientIndexBufferData& tib = submit_->transient_index_buffers_.at(handle);
    submit_->current_item.ib = transient_ib;
    submit_->current_item.ib_offset = (uint)(tib.data - submit_->transient_ib_storage.data);
}

ShaderHandle RHIRenderer::createShader(ShaderStage stage, const void* data, u32 size) {
    auto handle = shader_handle_.next();
    submitPreFrameCommand(cmd::CreateShader{handle, stage, Memory{data, size}});
    return handle;
}

void RHIRenderer::deleteShader(ShaderHandle handle) {
    submitPostFrameCommand(cmd::DeleteShader{handle});
}

ProgramHandle RHIRenderer::createProgram() {
    auto handle = program_handle_.next();
    submitPreFrameCommand(cmd::CreateProgram{handle});
    return handle;
}

void RHIRenderer::attachShader(ProgramHandle program, ShaderHandle shader) {
    submitPreFrameCommand(cmd::AttachShader{program, shader});
}

void RHIRenderer::linkProgram(ProgramHandle program) {
    submitPreFrameCommand(cmd::LinkProgram{program});
}

void RHIRenderer::deleteProgram(ProgramHandle program) {
    submitPostFrameCommand(cmd::DeleteProgram{program});
}

void RHIRenderer::setUniform(const String& uniform_name, int value) {
    setUniform(uniform_name, UniformData{value});
}

void RHIRenderer::setUniform(const String& uniform_name, float value) {
    setUniform(uniform_name, UniformData{value});
}

void RHIRenderer::setUniform(const String& uniform_name, const Vec2& value) {
    setUniform(uniform_name, UniformData{value});
}

void RHIRenderer::setUniform(const String& uniform_name, const Vec3& value) {
    setUniform(uniform_name, UniformData{value});
}

void RHIRenderer::setUniform(const String& uniform_name, const Vec4& value) {
    setUniform(uniform_name, UniformData{value});
}

void RHIRenderer::setUniform(const String& uniform_name, const Mat3& value) {
    setUniform(uniform_name, UniformData{value});
}

void RHIRenderer::setUniform(const String& uniform_name, const Mat4& value) {
    setUniform(uniform_name, UniformData{value});
}

void RHIRenderer::setUniform(const String& uniform_name, UniformData data) {
    submit_->current_item.uniforms[uniform_name] = data;
}

TextureHandle RHIRenderer::createTexture2D(u16 width, u16 height, TextureFormat format,
                                        const void* data, u32 size) {
    auto handle = texture_handle_.next();
    texture_data_[handle] = {width, height, format};
    submitPreFrameCommand(cmd::CreateTexture2D{handle, width, height, format, Memory{data, size}});
    return handle;
}

void RHIRenderer::setTexture(TextureHandle handle, uint sampler_unit) {
    // TODO: check precondition: texture_unit < MAX_TEXTURE_UNITS
    submit_->current_item.textures[sampler_unit].handle = handle;
}

void RHIRenderer::deleteTexture(TextureHandle handle) {
    texture_data_.erase(handle);
    submitPostFrameCommand(cmd::DeleteTexture{handle});
}

FrameBufferHandle RHIRenderer::createFrameBuffer(u16 width, u16 height, TextureFormat format) {
    auto handle = frame_buffer_handle_.next();
    auto texture_handle = createTexture2D(width, height, format, nullptr, 0);
    frame_buffer_textures_[handle] = {texture_handle};
    submitPreFrameCommand(cmd::CreateFrameBuffer{handle, width, height, {texture_handle}});
    return handle;
}

FrameBufferHandle RHIRenderer::createFrameBuffer(Vector<TextureHandle> textures) {
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

TextureHandle RHIRenderer::getFrameBufferTexture(FrameBufferHandle handle, uint index) {
    auto& textures = frame_buffer_textures_.at(handle);
    return textures[index];
}

void RHIRenderer::deleteFrameBuffer(FrameBufferHandle handle) {
    frame_buffer_textures_.erase(handle);
    submitPostFrameCommand(cmd::DeleteFrameBuffer{handle});
}

void RHIRenderer::setViewClear(uint view, const Colour& colour) {
    submit_->view(view).clear_colour = colour;
}

void RHIRenderer::setViewFrameBuffer(uint view, FrameBufferHandle handle) {
    submit_->view(view).frame_buffer = handle;
}

void RHIRenderer::setStateEnable(RenderState state) {
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

void RHIRenderer::setStateDisable(RenderState state) {
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

void RHIRenderer::setStateCullFrontFace(CullFrontFace front_face) {
    submit_->current_item.cull_front_face = front_face;
}

void RHIRenderer::setStatePolygonMode(PolygonMode polygon_mode) {
    submit_->current_item.polygon_mode = polygon_mode;
}

void RHIRenderer::setStateBlendEquation(BlendEquation equation, BlendFunc src, BlendFunc dest) {
    setStateBlendEquation(equation, src, dest, equation, src, dest);
}

void RHIRenderer::setStateBlendEquation(BlendEquation equation_rgb, BlendFunc src_rgb,
                                     BlendFunc dest_rgb, BlendEquation equation_a, BlendFunc src_a,
                                     BlendFunc dest_a) {
    submit_->current_item.blend_equation_rgb = equation_rgb;
    submit_->current_item.blend_src_rgb = src_rgb;
    submit_->current_item.blend_dest_rgb = dest_rgb;
    submit_->current_item.blend_equation_a = equation_a;
    submit_->current_item.blend_src_a = src_a;
    submit_->current_item.blend_dest_a = dest_a;
}

void RHIRenderer::setColourWrite(bool write_enabled) {
    submit_->current_item.colour_write = write_enabled;
}

void RHIRenderer::setDepthWrite(bool write_enabled) {
    submit_->current_item.depth_write = write_enabled;
}

void RHIRenderer::setScissor(u16 x, u16 y, u16 width, u16 height) {
    submit_->current_item.scissor_enabled = true;
    submit_->current_item.scissor_x = x;
    submit_->current_item.scissor_y = y;
    submit_->current_item.scissor_width = width;
    submit_->current_item.scissor_height = height;
}

void RHIRenderer::submit(uint view, ProgramHandle program) {
    submit(view, program, 0);
}

void RHIRenderer::submit(uint view, ProgramHandle program, uint vertex_count) {
    submit(view, program, vertex_count, 0);
}

void RHIRenderer::submit(uint view, ProgramHandle program, uint vertex_count, uint offset) {
    auto& item = submit_->current_item;
    item.program = program;
    item.primitive_count = vertex_count / 3;
    if (vertex_count > 0) {
        if (item.ib.isValid()) {
            IndexBufferType type = index_buffer_types_.at(item.ib);
            item.ib_offset += offset * (type == IndexBufferType::U16 ? sizeof(u16) : sizeof(u32));
        } else {
            const VertexDecl& decl = vertex_buffer_decl_.at(item.vb);
            item.vb_offset += offset * decl.stride();
        }
    }
    submit_->view(view).render_items.emplace_back(std::move(item));
    item.clear();
}

void RHIRenderer::frame() {
    // If we are rendering in multithreaded mode, wait for the render thread.
    if (use_render_thread_) {
        // If the rendering thread is doing nothing, print a warning and give up.
        if (shared_rt_finished_) {
            log().warn("Rendering thread has finished running. Sending shutdown signal.");
            module<EventSystem>()->triggerEvent(makeShared<ExitEvent>());
            return;
        }

        // Wait for render thread.
        shared_frame_barrier_.wait();

        // Wait for frame swap, then reset swapped_frames_. This has no race here, because the
        // render thread will not modify "swapped_frames_" again until after this thread hits the
        // barrier.
        UniqueLock<Mutex> lock{swap_mutex_};
        swap_cv_.wait(lock, [this] { return swapped_frames_; });
        swapped_frames_ = false;
    } else {
        if (is_first_frame_) {
            is_first_frame_ = false;
        }
        if (!renderFrame(submit_)) {
            module<EventSystem>()->triggerEvent(makeShared<ExitEvent>());
            log().warn("Rendering failed. Sending shutdown signal.");
            return;
        }
    }

    // Update window events.
    shared_render_context_->processEvents();
    if (shared_render_context_->isWindowClosed()) {
        log().info("Window closed. Sending shutdown signal.");
        module<EventSystem>()->triggerEvent(makeShared<ExitEvent>());
    }
}

Vec2i RHIRenderer::windowSize() const {
    return shared_render_context_->windowSize();
}

Vec2 RHIRenderer::windowScale() const {
    return shared_render_context_->windowScale();
}

Vec2i RHIRenderer::backbufferSize() const {
    return shared_render_context_->backbufferSize();
}

void RHIRenderer::submitPreFrameCommand(RenderCommand&& command) {
    submit_->commands_pre.emplace_back(std::move(command));
}

void RHIRenderer::submitPostFrameCommand(RenderCommand&& command) {
    submit_->commands_post.emplace_back(std::move(command));
}

void RHIRenderer::renderThread() {
    shared_render_context_->startRendering();

    while (!shared_rt_should_exit_) {
        // Skip rendering first frame (as this will be a no-op, but causes a crash as transient
        // buffers are not created yet).
        if (is_first_frame_) {
            is_first_frame_ = false;
        } else {
            if (!renderFrame(render_)) {
                shared_rt_should_exit_ = true;
            }
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

    shared_render_context_->stopRendering();
}

bool RHIRenderer::renderFrame(Frame* frame) {
    // Hand off commands to the render context.
    shared_render_context_->processCommandList(frame->commands_pre);
    if (!shared_render_context_->frame(frame)) {
        return false;
    }
    shared_render_context_->processCommandList(frame->commands_post);

    // Clear the frame state.
    frame->current_item.clear();
    for (auto& view : frame->views) {
        view.clear();
    }
    frame->view(0).frame_buffer = FrameBufferHandle{0};
    frame->commands_pre.clear();
    frame->commands_post.clear();
    frame->transient_vb_storage.size = 0;
    frame->transient_ib_storage.size = 0;
    frame->transient_vertex_buffers_.clear();
    frame->next_transient_vertex_buffer_handle_ = TransientVertexBufferHandle{0};
    frame->transient_index_buffers_.clear();
    frame->next_transient_index_buffer_handle_ = TransientIndexBufferHandle{0};
#ifdef DW_DEBUG
    frame->updated_vertex_buffers.clear();
    frame->updated_index_buffers.clear();
#endif

    return true;
}

uint RHIRenderer::backbufferView() const {
    for (uint view_index = 0; view_index < submit_->views.size(); ++view_index) {
        if (submit_->views[view_index].frame_buffer.internal() == 0) {
            return view_index;
        }
    }
    log().error("getBackbufferView() returned -1.");
    return -1;
}
}  // namespace rhi
}  // namespace dw
