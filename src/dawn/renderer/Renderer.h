/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

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

// Memory.
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
    void* data_;
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
enum class IndexBufferType {
    U16,
    U32
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

    struct TextureBinding {
        TextureHandle handle;
    };

    // Vertices
    VertexBufferHandle vb;
    uint vertex_count;
    IndexBufferHandle ib;
    uint index_count;
    ProgramHandle program;
    TextureBinding textures[MAX_TEXTURE_SAMPLERS];
};

// Frame.
struct Frame {
    Frame() {
        current_item.clear();
    }

    RenderItem current_item;
    Vector<RenderItem> render_items;
    Vector<RenderCommand> commands;
};

// Abstract rendering context.
class DW_API RenderContext : public Object {
public:
    DW_OBJECT(RenderContext);

    RenderContext(Context* context);
    virtual ~RenderContext() = default;
    virtual void processCommand(RenderCommand& command) = 0;
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

    // Create texture.
    TextureHandle createTexture2D();
    void setTexture(TextureHandle handle, uint texture_unit);
    void deleteTexture(TextureHandle handle);

    /// Clear.
    void clear(const Vec3& colour);

    /// Draw. Based off: https://github.com/bkaradzic/bgfx/blob/master/src/bgfx.cpp#L854
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
    HandleGenerator<IndexBufferHandle > index_buffer_handle_;
    HandleGenerator<ShaderHandle> shader_handle_;
    HandleGenerator<ProgramHandle> program_handle_;
    HandleGenerator<TextureHandle> texture_handle_;

    // Shared.
    Atomic<bool> should_exit_;
    Mutex submit_lock_;
    Mutex render_lock_;
    Frame frames_[2];
    Frame* submit_;
    Frame* render_;
    void addCommand(RenderCommand command);

    // Renderer.
    UniquePtr<RenderContext> r_render_context_;

    // Render thread proc.
    void renderThread();
};
}  // namespace dw
