/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "core/Concurrency.h"
#include "renderer/RenderTask.h"
#include "renderer/VertexDecl.h"
#include "renderer/Window.h"

#define MAX_TEXTURE_SAMPLERS 8

namespace dw {

// Handles.
using VertexBufferHandle = u16;
using IndexBufferHandle = u16;
using ShaderHandle = u16;
using ProgramHandle = u16;
using TextureHandle = u16;

// Handle generator.
template <typename Handle> class HandleGenerator {
public:
    HandleGenerator() : next_{1} {
    }
    ~HandleGenerator() = default;

    Handle next() {
        return next_++;
    }

private:
    Handle next_;
};

// Shader type.
enum class ShaderType { Vertex, Geometry, Fragment };

// A blob of memory.
class Memory {
public:
    Memory(const void* data, uint size);
    ~Memory();

    // Non-copyable.
    Memory(const Memory&) = delete;
    Memory& operator=(const Memory&) = delete;

    // Movable.
    Memory(Memory&&) noexcept;
    Memory& operator=(Memory&&) noexcept;

    void* data() const;
    uint size() const;

private:
    byte* data_;
    uint size_;
};

/*
-        CreateVertexBuffer,
-        SetVertexBuffer,
-        DeleteVertexBuffer,
-        CreateIndexBuffer,
-        SetIndexBuffer,
-        DeleteIndexBuffer,
-        CreateShader,
-        DeleteShader,
-        CreateProgram,
-        AttachShader,
-        LinkProgram,
-        DeleteProgram,
-        CreateTexture2D,
-        SetTexture,
-        DeleteTexture,
-        Clear,
-        Submit
 */

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
    ShaderType type;
    const String source;
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
            cmd::DeleteTexture>;
// clang-format on

// Current render state.
struct RenderItem {
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
    TextureBinding textures[MAX_TEXTURE_SAMPLERS];
};

// Frame.
struct Frame {
    Frame() {
        current_item.clear();
    }

    RenderItem current_item;
    Vector<RenderItem> render_items;
    Vector<RenderCommand> commands_pre;
    Vector<RenderCommand> commands_post;
};

// Abstract rendering context.
class DW_API RenderContext : public Object {
public:
    DW_OBJECT(RenderContext);

    RenderContext(Context* context);
    virtual ~RenderContext() = default;
    virtual void processCommandList(Vector<RenderCommand>& command_list) = 0;
    virtual void submit(const Vector<RenderItem>& items) = 0;
};

// Low level renderer.
// Based off: https://github.com/bkaradzic/bgfx/blob/master/src/bgfx_p.h#L2297
class DW_API Renderer : public Object {
public:
    DW_OBJECT(Renderer)

    Renderer(Context* context, Window* window);
    ~Renderer();

    /// Create vertex buffer.
    VertexBufferHandle createVertexBuffer(const void* data, uint size, const VertexDecl& decl);
    void setVertexBuffer(VertexBufferHandle handle);
    void deleteVertexBuffer(VertexBufferHandle handle);

    /// Create index buffer.
    IndexBufferHandle createIndexBuffer(const void* data, uint size, IndexBufferType type);
    void setIndexBuffer(IndexBufferHandle handle);
    void deleteIndexBuffer(IndexBufferHandle handle);

    /// Create shader.
    ShaderHandle createShader(ShaderType type, const String& source);
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
    void deleteTexture(TextureHandle handle);

    /// Clear.
    void clear(const Vec3& colour);

    /// Update uniform and draw state, but submit no geometry.
    void submit(ProgramHandle program);

    /// Update uniform and draw state, then draw. Based off:
    /// https://github.com/bkaradzic/bgfx/blob/master/src/bgfx.cpp#L854
    void submit(ProgramHandle program, uint vertex_count);

    /// Push render task.
    void pushRenderTask(RenderTask&& task);

    /// Render a single frame.
    void frame();

private:
    u16 width_, height_;
    Vector<RenderTask> render_tasks_;  // deprecated.

    GLFWwindow* window_;
    Thread render_thread_;

    // Main thread.
    HandleGenerator<VertexBufferHandle> vertex_buffer_handle_;
    HandleGenerator<IndexBufferHandle> index_buffer_handle_;
    HandleGenerator<ShaderHandle> shader_handle_;
    HandleGenerator<ProgramHandle> program_handle_;
    HandleGenerator<TextureHandle> texture_handle_;

    // Shared.
    Atomic<bool> should_exit_;

    Barrier frame_barrier_;
    Mutex swap_mutex_;
    ConditionVariable swap_cv_;
    bool swapped_frames_;

    Frame frames_[2];
    Frame* submit_;
    Frame* render_;

    // Add a command to the submit thread.
    void submitPreFrameCommand(RenderCommand command);
    void submitPostFrameCommand(RenderCommand command);

    // Renderer.
    UniquePtr<RenderContext> r_render_context_;

    // Render thread proc.
    void renderThread();
};
}  // namespace dw
