/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Renderer.h"

namespace dw {
class DW_API GLRenderer : public RendererAPI {
public:
    DW_OBJECT(GLRenderer);

    GLRenderer(Context* context);
    virtual ~GLRenderer();

    void processCommand(RenderCommand& command) override;

private:
    HashMap<VertexBufferHandle, GLuint> r_vertex_buffer_map_;
    HashMap<ShaderHandle, GLuint> r_shader_map_;
    HashMap<ProgramHandle, GLuint> r_program_map_;
};
}  // namespace dw