/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Renderer.h"

namespace dw {
class DW_API GLRenderContext : public RenderContext {
public:
    DW_OBJECT(GLRenderContext);

    GLRenderContext(Context* context);
    virtual ~GLRenderContext();

    void processCommandList(Vector<RenderCommand>& command_list) override;
    void submit(const Vector<RenderItem>& items) override;

    void operator()(const cmd::CreateVertexBuffer& c);
    void operator()(const cmd::DeleteVertexBuffer& c);
    void operator()(const cmd::CreateIndexBuffer& c);
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
    // Vertex and index buffers.
    HashMap<VertexBufferHandle, GLuint> r_vertex_buffer_map_;
    HashMap<IndexBufferHandle, Pair<GLuint, GLenum>> r_index_buffer_map_;

    // Shaders programs.
    struct ProgramData {
        GLuint program;
        HashMap<String, GLint> uniform_location_map;
    };
    HashMap<ShaderHandle, GLuint> r_shader_map_;
    HashMap<ProgramHandle, ProgramData> r_program_map_;

    // Textures.
    HashMap<TextureHandle, GLuint> r_texture_map_;

    // Frame buffers.
    struct FrameBufferData {
        GLuint frame_buffer;
        GLuint depth_render_buffer;
        Vector<TextureHandle> textures;
    };
    HashMap<FrameBufferHandle, FrameBufferData> r_frame_buffer_map_;
};
}  // namespace dw