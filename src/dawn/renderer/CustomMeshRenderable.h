/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Renderable.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"

namespace dw {
class DW_API CustomMeshRenderable : public Renderable {
public:
    DW_OBJECT(CustomMeshRenderable);

    CustomMeshRenderable(Context* context, SharedPtr<VertexBuffer> vertexBuffer,
                         SharedPtr<IndexBuffer> indexBuffer);
    ~CustomMeshRenderable();

    void draw(Renderer* renderer, uint view, const Mat4& modelMatrix) override;

    const VertexBuffer* vertexBuffer() const;
    const IndexBuffer* indexBuffer() const;

private:
    SharedPtr<VertexBuffer> vertex_buffer_;
    SharedPtr<IndexBuffer> index_buffer_;
};
}  // namespace dw
