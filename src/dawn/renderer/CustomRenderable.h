/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "renderer/Renderable.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"
#include <dawn-gfx/MeshBuilder.h>

namespace dw {
class DW_API CustomRenderable : public Object, public Renderable {
public:
    DW_OBJECT(CustomRenderable);

    class Builder : public Object {
    public:
        DW_OBJECT(Builder);

        explicit Builder(Context* ctx);

        Builder& normals(bool normals);
        Builder& texcoords(bool texcoords);

        SharedPtr<CustomRenderable> createPlane(float width, float height);
        SharedPtr<CustomRenderable> createBox(float half_size);
        SharedPtr<CustomRenderable> createSphere(float radius, uint rings = 25, uint segments = 25);

    private:
        gfx::MeshBuilder mesh_builder_;
    };

    CustomRenderable(Context* ctx, gfx::Mesh gfx_mesh);
    CustomRenderable(Context* ctx, SharedPtr<VertexBuffer> vertex_buffer,
                         SharedPtr<IndexBuffer> index_buffer);
    ~CustomRenderable() override;

    void draw(Renderer* renderer, uint view, detail::Transform& camera, const Mat4& model_matrix,
              const Mat4& view_projection_matrix) override;

    VertexBuffer* vertexBuffer() const;
    IndexBuffer* indexBuffer() const;

private:
    SharedPtr<VertexBuffer> vertex_buffer_;
    SharedPtr<IndexBuffer> index_buffer_;
};
}  // namespace dw
