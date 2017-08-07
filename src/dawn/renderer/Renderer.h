/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "core/Concurrency.h"
#include "core/Handle.h"
#include "math/Colour.h"
#include "renderer/VertexDecl.h"

#define MAX_TEXTURE_SAMPLERS 8

namespace dw {
// Handles.
namespace detail {
struct VertexBufferTag {};
struct IndexBufferTag {};
struct ShaderTag {};
struct ProgramTag {};
struct TextureTag {};
struct FrameBufferTag {};
}  // namespace detail
using VertexBufferHandle = Handle<detail::VertexBufferTag, -1>;
using IndexBufferHandle = Handle<detail::IndexBufferTag, -1>;
using ShaderHandle = Handle<detail::ShaderTag, -1>;
using ProgramHandle = Handle<detail::ProgramTag, -1>;
using TextureHandle = Handle<detail::TextureTag, -1>;
using FrameBufferHandle = Handle<detail::FrameBufferTag, -1>;

// Shader type.
enum class ShaderStage { Vertex, Geometry, Fragment };

// A blob of memory.
class Memory {
public:
    Memory();
    Memory(const void* data, uint size);
    ~Memory();

    // Copyable.
    Memory(const Memory&) noexcept;
    Memory& operator=(const Memory&) noexcept;

    // Movable.
    Memory(Memory&&) noexcept;
    Memory& operator=(Memory&&) noexcept;

    void* data() const;
    uint size() const;

private:
    byte* data_;
    uint size_;
};

// Index buffer type.
enum class IndexBufferType { U16, U32 };

// Texture format.
/*
 * RGBA16S
 * ^   ^ ^
 * |   | +-- [ ]Unorm
 * |   |     [F]loat
 * |   |     [S]norm
 * |   |     [I]nt
 * |   |     [U]int
 * |   +---- Number of bits per component
 * +-------- Components
 */
enum class TextureFormat {
    // Colour formats.
    A8,
    R8,
    R8I,
    R8U,
    R8S,
    R16,
    R16I,
    R16U,
    R16F,
    R16S,
    R32I,
    R32U,
    R32F,
    RG8,
    RG8I,
    RG8U,
    RG8S,
    RG16,
    RG16I,
    RG16U,
    RG16F,
    RG16S,
    RG32I,
    RG32U,
    RG32F,
    RGB8,
    RGB8I,
    RGB8U,
    RGB8S,
    BGRA8,
    RGBA8,
    RGBA8I,
    RGBA8U,
    RGBA8S,
    RGBA16,
    RGBA16I,
    RGBA16U,
    RGBA16F,
    RGBA16S,
    RGBA32I,
    RGBA32U,
    RGBA32F,
    R5G6B5,
    RGBA4,
    RGB5A1,
    RGB10A2,
    RG11B10F,
    // Depth formats.
    D16,
    D24,
    D24S8,
    D32,
    D16F,
    D24F,
    D32F,
    D0S8,
    Count
};

// Render states.
enum class RenderState { CullFace, Depth, Blending };
enum class CullFrontFace { CCW, CW };
enum class BlendFunc {
    Zero,
    One,
    SrcColor,
    OneMinusSrcColor,
    DstColor,
    OneMinusDstColor,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstAlpha,
    OneMinusDstAlpha,
    ConstantColor,
    OneMinusConstantColor,
    ConstantAlpha,
    OneMinusConstantAlpha,
    SrcAlphaSaturate
};
enum class BlendEquation { Add, Subtract, ReverseSubtract, Min, Max };

// Render commands.
namespace cmd {
struct CreateVertexBuffer {
    VertexBufferHandle handle;
    Memory data;
    VertexDecl decl;
};

struct DeleteVertexBuffer {
    VertexBufferHandle handle;
};

struct CreateIndexBuffer {
    IndexBufferHandle handle;
    Memory data;
    IndexBufferType type;
};

struct DeleteIndexBuffer {
    IndexBufferHandle handle;
};

struct CreateShader {
    ShaderHandle handle;
    ShaderStage stage;
    Memory data;
};

struct DeleteShader {
    ShaderHandle handle;
};

struct CreateProgram {
    ProgramHandle handle;
};

struct AttachShader {
    ProgramHandle handle;
    ShaderHandle shader_handle;
};

struct LinkProgram {
    ProgramHandle handle;
};

struct DeleteProgram {
    ProgramHandle handle;
};

struct CreateTexture2D {
    TextureHandle handle;
    u16 width;
    u16 height;
    TextureFormat format;
    // TODO: Support custom mips.
    // TODO: Support different filtering.
    Memory data;
};

struct DeleteTexture {
    TextureHandle handle;
};

struct CreateFrameBuffer {
    FrameBufferHandle handle;
    u16 width;
    u16 height;
    // TODO: do we own textures or does the user own textures? add a flag?
    Vector<TextureHandle> textures;
};

struct DeleteFrameBuffer {
    FrameBufferHandle handle;
};
}  // namespace cmd

// clang-format off
using RenderCommand =
    Variant<cmd::CreateVertexBuffer,
            cmd::DeleteVertexBuffer,
            cmd::CreateIndexBuffer,
            cmd::DeleteIndexBuffer,
            cmd::CreateShader,
            cmd::DeleteShader,
            cmd::CreateProgram,
            cmd::AttachShader,
            cmd::LinkProgram,
            cmd::DeleteProgram,
            cmd::CreateTexture2D,
            cmd::DeleteTexture,
            cmd::CreateFrameBuffer,
            cmd::DeleteFrameBuffer>;
// clang-format on

// Current render state.
struct RenderItem {
    RenderItem();
    void clear();

    using UniformData = Variant<int, float, Vec2, Vec3, Vec4, Mat3, Mat4>;

    struct TextureBinding {
        TextureHandle handle;
    };

    // Vertices and Indices.
    VertexBufferHandle vb;
    IndexBufferHandle ib;
    uint primitive_count;

    // Shader program and parameters.
    ProgramHandle program;
    HashMap<String, UniformData> uniforms;
    Array<TextureBinding, MAX_TEXTURE_SAMPLERS> textures;

    // Render state.
    bool depth_enabled;
    bool cull_face_enabled;
    CullFrontFace cull_front_face;
    bool blend_enabled;
    BlendEquation blend_equation_rgb;
    BlendFunc blend_src_rgb;
    BlendFunc blend_dest_rgb;
    BlendEquation blend_equation_a;
    BlendFunc blend_src_a;
    BlendFunc blend_dest_a;
};

// View.
struct View {
    View() : clear_colour{0.0f, 0.0f, 0.0f, 1.0f}, frame_buffer{0} {
    }
    Colour clear_colour;
    FrameBufferHandle frame_buffer;
    Vector<RenderItem> render_items;
};

// Frame.
struct Frame {
    Frame() {
        current_item.clear();
    }

    View& view(uint view_index) {
        if (views.size() <= view_index) {
            views.resize(view_index + 1);
        }
        return views[view_index];
    }

    RenderItem current_item;
    Vector<View> views;
    Vector<RenderCommand> commands_pre;
    Vector<RenderCommand> commands_post;
};

// Abstract rendering context.
class DW_API RenderContext : public Object {
public:
    DW_OBJECT(RenderContext);

    RenderContext(Context* context);
    virtual ~RenderContext() = default;

    // Window management. Executed on the main thread.
    virtual void createWindow(u16 width, u16 height, const String& title) = 0;
    virtual void destroyWindow() = 0;
    virtual void processEvents() = 0;
    virtual bool isWindowClosed() const = 0;

    // Command buffer processing. Executed on the render thread.
    virtual void startRendering() = 0;
    virtual void processCommandList(Vector<RenderCommand>& command_list) = 0;
    virtual bool frame(const Vector<View>& views) = 0;
};

// Low level renderer.
// Based off: https://github.com/bkaradzic/bgfx/blob/master/src/bgfx_p.h#L2297
class DW_API Renderer : public Object {
public:
    DW_OBJECT(Renderer)

    Renderer(Context* context);
    ~Renderer();

    /// Initialise.
    void init(u16 width, u16 height, const String& title);

    /// Create vertex buffer.
    VertexBufferHandle createVertexBuffer(const void* data, uint size, const VertexDecl& decl);
    void setVertexBuffer(VertexBufferHandle handle);
    void deleteVertexBuffer(VertexBufferHandle handle);

    /// Create index buffer.
    IndexBufferHandle createIndexBuffer(const void* data, uint size, IndexBufferType type);
    void setIndexBuffer(IndexBufferHandle handle);
    void deleteIndexBuffer(IndexBufferHandle handle);

    /// Create shader.
    ShaderHandle createShader(ShaderStage type, const void* data, uint size);
    void deleteShader(ShaderHandle handle);

    /// Create program.
    ProgramHandle createProgram();
    void attachShader(ProgramHandle program, ShaderHandle shader);
    void linkProgram(ProgramHandle program);
    void deleteProgram(ProgramHandle program);

    /// Uniforms.
    void setUniform(const String& uniform_name, int value);
    void setUniform(const String& uniform_name, float value);
    void setUniform(const String& uniform_name, const Vec2& value);
    void setUniform(const String& uniform_name, const Vec3& value);
    void setUniform(const String& uniform_name, const Vec4& value);
    void setUniform(const String& uniform_name, const Mat3& value);
    void setUniform(const String& uniform_name, const Mat4& value);

    // Create texture.
    TextureHandle createTexture2D(u16 width, u16 height, TextureFormat format, const void* data,
                                  u32 size);
    void setTexture(TextureHandle handle, uint sampler_unit);
    // get texture information.
    void deleteTexture(TextureHandle handle);

    // Framebuffer.
    FrameBufferHandle createFrameBuffer(u16 width, u16 height, TextureFormat format);
    FrameBufferHandle createFrameBuffer(Vector<TextureHandle> textures);
    TextureHandle getFrameBufferTexture(FrameBufferHandle handle, uint index);
    void deleteFrameBuffer(FrameBufferHandle handle);

    /// View.
    void setViewClear(uint view, const Colour& colour);
    void setViewFrameBuffer(uint view, FrameBufferHandle handle);

    /// Update state.
    void setStateEnable(RenderState state);
    void setStateDisable(RenderState state);
    void setStateCullFrontFace(CullFrontFace front_face);
    void setStateBlendEquation(BlendEquation equation, BlendFunc src, BlendFunc dest);
    void setStateBlendEquation(BlendEquation equation_rgb, BlendFunc src_rgb, BlendFunc dest_rgb,
                               BlendEquation equation_a, BlendFunc src_a, BlendFunc dest_a);

    /// Update uniform and draw state, but submit no geometry.
    void submit(uint view, ProgramHandle program);

    /// Update uniform and draw state, then draw. Based off:
    /// https://github.com/bkaradzic/bgfx/blob/master/src/bgfx.cpp#L854
    void submit(uint view, ProgramHandle program, uint vertex_count);

    /// Render a single frame.
    void frame();

private:
    u16 width_, height_;
    String window_title_;

    Thread render_thread_;

    // Main thread.
    HandleGenerator<VertexBufferHandle> vertex_buffer_handle_;
    HandleGenerator<IndexBufferHandle> index_buffer_handle_;
    HandleGenerator<ShaderHandle> shader_handle_;
    HandleGenerator<ProgramHandle> program_handle_;
    HandleGenerator<TextureHandle> texture_handle_;
    HandleGenerator<FrameBufferHandle> frame_buffer_handle_;

    // Textures.
    struct TextureData {
        u16 width;
        u16 height;
        TextureFormat format;
    };
    HashMap<TextureHandle, TextureData> texture_data_;

    // Framebuffers.
    HashMap<FrameBufferHandle, Vector<TextureHandle>> frame_buffer_textures_;

    // Shared.
    Atomic<bool> shared_rt_should_exit_;
    bool shared_rt_finished_;
    Barrier shared_frame_barrier_;
    Mutex swap_mutex_;
    ConditionVariable swap_cv_;
    bool swapped_frames_;

    Frame frames_[2];
    Frame* submit_;
    Frame* render_;

    // Add a command to the submit thread.
    void submitPreFrameCommand(RenderCommand&& command);
    void submitPostFrameCommand(RenderCommand&& command);

    // Renderer.
    UniquePtr<RenderContext> shared_render_context_;

    // Render thread proc.
    void renderThread();
};
}  // namespace dw
