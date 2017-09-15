/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Renderer.h"
#include "renderer/api/GL.h"

namespace dw {
class DW_API GLRenderContext : public RenderContext {
public:
    DW_OBJECT(GLRenderContext);

    GLRenderContext(Context* context);
    virtual ~GLRenderContext();

    // Window management. Executed on the main thread.
    void createWindow(u16 width, u16 height, const String& title) override;
    void destroyWindow() override;
    void processEvents() override;
    bool isWindowClosed() const override;

    // Command buffer processing. Executed on the render thread.
    void startRendering() override;
    void stopRendering() override;
    void processCommandList(Vector<RenderCommand>& command_list) override;
    bool frame(const Frame* frame) override;

    // Variant walker methods. Executed on the render thread.
    void operator()(const cmd::CreateVertexBuffer& c);
    void operator()(const cmd::UpdateVertexBuffer& c);
    void operator()(const cmd::DeleteVertexBuffer& c);
    void operator()(const cmd::CreateIndexBuffer& c);
    void operator()(const cmd::UpdateIndexBuffer& c);
    void operator()(const cmd::DeleteIndexBuffer& c);
    void operator()(const cmd::CreateShader& c);
    void operator()(const cmd::DeleteShader& c);
    void operator()(const cmd::CreateProgram& c);
    void operator()(const cmd::AttachShader& c);
    void operator()(const cmd::LinkProgram& c);
    void operator()(const cmd::DeleteProgram& c);
    void operator()(const cmd::CreateTexture2D& c);
    void operator()(const cmd::DeleteTexture& c);
    void operator()(const cmd::CreateFrameBuffer& c);
    void operator()(const cmd::DeleteFrameBuffer& c);
    template <typename T> void operator()(const T& c) {
        static_assert(!std::is_same<T, T>::value, "Unimplemented RenderCommand");
    }

private:
    GLFWwindow* window_;
    u16 backbuffer_width_;
    u16 backbuffer_height_;

    GLuint vao_;
    VertexDecl current_vertex_decl;

    // Vertex and index buffers.
    struct VertexBufferData {
        GLuint vertex_buffer;
        VertexDecl decl;
        GLenum usage;
        size_t size;
    };
    struct IndexBufferData {
        GLuint element_buffer;
        GLenum type;
        GLenum usage;
        size_t size;
    };
    HashMap<VertexBufferHandle, VertexBufferData> vertex_buffer_map_;
    HashMap<IndexBufferHandle, IndexBufferData> index_buffer_map_;

    // Shaders and programs.
    struct ProgramData {
        GLuint program;
        HashMap<String, GLint> uniform_location_map;
    };
    HashMap<ShaderHandle, GLuint> shader_map_;
    HashMap<ProgramHandle, ProgramData> program_map_;

    // Textures.
    HashMap<TextureHandle, GLuint> texture_map_;

    // Frame buffers.
    struct FrameBufferData {
        GLuint frame_buffer;
        GLuint depth_render_buffer;
        u16 width;
        u16 height;
        Vector<TextureHandle> textures;
    };
    HashMap<FrameBufferHandle, FrameBufferData> frame_buffer_map_;

    // Helper functions.
    void setupVertexArrayAttributes(const VertexDecl& decl, uint vb_offset);
};
}  // namespace dw
