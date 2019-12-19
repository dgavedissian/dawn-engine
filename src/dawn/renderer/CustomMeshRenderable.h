/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "renderer/Renderable.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"
#include <dawn-gfx/TriangleBuffer.h>

namespace dw {
class DW_API CustomMeshRenderable : public Object, public Renderable {
public:
    DW_OBJECT(CustomMeshRenderable);

    CustomMeshRenderable(Context* ctx, gfx::Mesh gfx_mesh);
    CustomMeshRenderable(Context* ctx, SharedPtr<VertexBuffer> vertex_buffer,
                         SharedPtr<IndexBuffer> index_buffer);
    ~CustomMeshRenderable();

    void draw(Renderer* renderer, uint view, detail::Transform& camera, const Mat4& model_matrix,
              const Mat4& view_projection_matrix) override;

    VertexBuffer* vertexBuffer() const;
    IndexBuffer* indexBuffer() const;

private:
    SharedPtr<VertexBuffer> vertex_buffer_;
    SharedPtr<IndexBuffer> index_buffer_;
};
}  // namespace dw
