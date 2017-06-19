/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/RenderTask.h"
#include "renderer/GL.h"
#include "Window.h"

namespace dw {

// Handles.
using ShaderHandle = uint;
using ProgramHandle = uint;
using VertexBufferHandle = uint;

// Shader type.
enum class ShaderType { VERTEX, FRAGMENT };

// Render command.
struct RenderCommand {
    enum class Type { CreateShader };

    Type type;
    union {
        struct {
            ShaderHandle handle;
            ShaderType type;
            String source;
        } create_shader;
    };
};

// Low level renderer.
class DW_API Renderer : public Object {
public:
    DW_OBJECT(Renderer)

    Renderer(Context* context, Window* window);
    ~Renderer();

    /// Create shader.
    ShaderHandle createShader(ShaderType type, const String& source);

    /// Create program.
    ProgramHandle createProgram();

    /// Draw.

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
    ShaderHandle next_shader_handle_;
    ProgramHandle next_program_handle_;

    // Shared.
    Atomic<bool> should_exit_;
    Vector<RenderCommand> command_buffer_[2];
    uint submit_command_buffer_;
    uint render_command_buffer_;

    // Render thread.
    HashMap<ShaderHandle, GLuint> r_shader_map_;
    HashMap<ProgramHandle, GLuint> r_program_map_;

    void pushCommand(RenderCommand command);
    void renderThread();
};
}  // namespace dw
