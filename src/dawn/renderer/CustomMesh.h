/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Renderable.h"
#include "renderer/GLVertexBuffer.h"
#include "renderer/GLIndexBuffer.h"

namespace dw {
class DW_API CustomMesh : public Renderable {
public:
    DW_OBJECT(CustomMesh);

    CustomMesh(Context* context, SharedPtr<GLVertexBuffer> vertexBuffer,
               SharedPtr<GLIndexBuffer> indexBuffer);
    ~CustomMesh();

    RenderTask draw(const Mat4& modelMatrix) override;

private:
    SharedPtr<GLVertexBuffer> vertex_buffer_;
    SharedPtr<GLIndexBuffer> index_buffer_;
};
}  // namespace dw
