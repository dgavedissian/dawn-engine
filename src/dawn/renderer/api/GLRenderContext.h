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

    void processCommand(RenderCommand& command) override;

    void operator()(const cmd::CreateVertexBuffer& c);
    void operator()(const cmd::SetVertexBuffer& c);
    void operator()(const cmd::CreateShader& c);
    void operator()(const cmd::CreateProgram& c);
    void operator()(const cmd::AttachShader& c);
    void operator()(const cmd::LinkProgram& c);
    void operator()(const cmd::Clear& c);
    void operator()(const cmd::Submit& c);

private:
    HashMap<VertexBufferHandle, GLuint> r_vertex_buffer_map_;
    HashMap<ShaderHandle, GLuint> r_shader_map_;
    HashMap<ProgramHandle, GLuint> r_program_map_;
};
}  // namespace dw