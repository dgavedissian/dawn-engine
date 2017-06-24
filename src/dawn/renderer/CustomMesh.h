/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Renderable.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"

namespace dw {
class DW_API CustomMesh : public Renderable {
public:
    DW_OBJECT(CustomMesh);

    CustomMesh(Context* context, SharedPtr<VertexBuffer> vertexBuffer,
               SharedPtr<GLIndexBuffer> indexBuffer);
    ~CustomMesh();

    RenderTask draw(const Mat4& modelMatrix) override;

private:
    SharedPtr<VertexBuffer> vertex_buffer_;
    SharedPtr<GLIndexBuffer> index_buffer_;
};
}  // namespace dw
