/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Renderable.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"

namespace dw {
class DW_API CustomMeshRenderable : public Object, public Renderable {
public:
    DW_OBJECT(CustomMeshRenderable);

    CustomMeshRenderable(Context* context, SharedPtr<VertexBuffer> vertex_buffer,
                         SharedPtr<IndexBuffer> index_buffer);
    ~CustomMeshRenderable();

    void draw(Renderer* renderer, uint view, Transform* camera, const Mat4& model_matrix,
              const Mat4& view_projection_matrix) override;

    VertexBuffer* vertexBuffer() const;
    IndexBuffer* indexBuffer() const;

private:
    SharedPtr<VertexBuffer> vertex_buffer_;
    SharedPtr<IndexBuffer> index_buffer_;
};
}  // namespace dw
