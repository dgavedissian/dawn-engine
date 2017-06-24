/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/RenderTask.h"
#include "renderer/VertexDecl.h"
#include "renderer/Window.h"

namespace dw {

// Handles.
using ShaderHandle = uint;
using ProgramHandle = uint;
using VertexBufferHandle = uint;

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

// Render command.
struct RenderCommand {
    enum class Type {
        CreateVertexBuffer,
        SetVertexBuffer,
        DeleteVertexBuffer,
        CreateIndexBuffer,
        SetIndexBuffer,
        DeleteIndexBuffer,
        CreateShader,
        DeleteShader,
        CreateProgram,
        AttachShader,
        LinkProgram,
        DeleteProgram,
        CreateTexture2D,
        SetTexture,
        DeleteTexture,
        Clear,
        Submit
    };

    Type type;
    union {
        struct {
            VertexBufferHandle handle;
            const byte* data;  // new
            uint size;
            VertexDecl* decl;  // new
        } create_vertex_buffer;
        struct {
            VertexBufferHandle handle;
        } set_vertex_buffer;
        struct {
            ShaderHandle handle;
            ShaderType type;
            const char* source;  // new
        } create_shader;
        struct {
            ProgramHandle handle;
        } create_program;
        struct {
            ProgramHandle handle;
            ShaderHandle shader_handle;
        } attach_shader;
        struct {
            ProgramHandle handle;
        } link_program;
        struct {
            float colour[3];
        } clear;
        struct {
            ProgramHandle handle;
            uint vertex_count;
        } submit;
    };
};

// Abstract rendering context.
class DW_API RenderContext : public Object {
public:
    DW_OBJECT(RenderContext);

    RenderContext(Context* context);
    virtual ~RenderContext() = default;
    virtual void processCommand(RenderCommand& command) = 0;
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

    /// Create shader.
    ShaderHandle createShader(ShaderType type, const String& source);

    /// Create program.
    ProgramHandle createProgram();
    void attachShader(ProgramHandle program, ShaderHandle shader);
    void linkProgram(ProgramHandle program);

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
    HandleGenerator<ShaderHandle> shader_handle_;
    HandleGenerator<ProgramHandle> program_handle_;

    // Shared.
    Atomic<bool> should_exit_;
    Mutex submit_lock_;
    Mutex render_lock_;
    Vector<RenderCommand> command_buffer_[2];
    uint submit_command_buffer_;
    uint render_command_buffer_;
    RenderCommand& addCommand(RenderCommand::Type type);

    // Renderer.
    UniquePtr<RenderContext> r_render_context_;

    // Render thread proc.
    void renderThread();
};
}  // namespace dw
