/*
* Dawn Engine
* Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
*/
#pragma once

#include "renderer/Renderable.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"

namespace dw {
class DW_API ManualMesh : public Renderable {
public:
    DW_OBJECT(ManualMesh);

    ManualMesh(Context* context, SharedPtr<VertexBuffer> vertexBuffer,
               SharedPtr<IndexBuffer> indexBuffer);
    ~ManualMesh();

    RenderTask draw(const Mat4& modelMatrix) override;

private:
    SharedPtr<VertexBuffer> vertex_buffer_;
    SharedPtr<IndexBuffer> index_buffer_;
};
}