/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "renderer/CustomRenderable.h"
#include "renderer/Program.h"
#include "renderer/Renderer.h"

namespace dw {
CustomRenderable::Builder::Builder(Context* ctx)
    : Object{ctx}, mesh_builder_{*module<Renderer>()->gfx()} {
}

CustomRenderable::Builder& CustomRenderable::Builder::normals(bool normals) {
    mesh_builder_.normals(normals);
    return *this;
}

CustomRenderable::Builder& CustomRenderable::Builder::texcoords(bool texcoords) {
    mesh_builder_.texcoords(texcoords);
    return *this;
}

CustomRenderable::Builder& CustomRenderable::Builder::tangents(bool tangents) {
    mesh_builder_.tangents(tangents);
    return *this;
}

SharedPtr<CustomRenderable> CustomRenderable::Builder::createPlane(float width, float height) {
    return makeShared<CustomRenderable>(context(), mesh_builder_.createPlane(width, height));
}

SharedPtr<CustomRenderable> CustomRenderable::Builder::createBox(float half_size) {
    return makeShared<CustomRenderable>(context(), mesh_builder_.createBox(half_size));
}

SharedPtr<CustomRenderable> CustomRenderable::Builder::createSphere(float radius, uint rings,
                                                                    uint segments) {
    return makeShared<CustomRenderable>(context(),
                                        mesh_builder_.createSphere(radius, rings, segments));
}

CustomRenderable::CustomRenderable(Context* ctx, gfx::Mesh gfx_mesh)
    : CustomRenderable{ctx, makeShared<VertexBuffer>(ctx, gfx_mesh.vb, gfx_mesh.vertex_count),
                       makeShared<IndexBuffer>(ctx, gfx_mesh.ib, gfx_mesh.index_count,
                                               gfx::IndexBufferType::U32)} {
}

CustomRenderable::CustomRenderable(Context* ctx, SharedPtr<VertexBuffer> vertex_buffer,
                                   SharedPtr<IndexBuffer> index_buffer)
    : Object{ctx}, vertex_buffer_{vertex_buffer}, index_buffer_{index_buffer} {
}

CustomRenderable::~CustomRenderable() {
}

void CustomRenderable::draw(Renderer* renderer, uint render_queue, detail::Transform&,
                            const Mat4& model_matrix, const Mat4& view_projection_matrix) {
    auto gfx = renderer->gfx();
    usize vertex_count =
        index_buffer_ ? index_buffer_->indexCount() : vertex_buffer_->vertexCount();
    gfx->setVertexBuffer(vertex_buffer_->internalHandle());
    if (index_buffer_) {
        gfx->setIndexBuffer(index_buffer_->internalHandle());
    }
    // TODO: Move this common "render vertex/index buffer + material" code somewhere to avoid
    // duplication with Mesh.
    // TODO: Support unset material.
    material_->applyRendererState(model_matrix, view_projection_matrix);
    gfx->submit(render_queue, material_->program()->internalHandle(), vertex_count);
}

VertexBuffer* CustomRenderable::vertexBuffer() const {
    return vertex_buffer_.get();
}

IndexBuffer* CustomRenderable::indexBuffer() const {
    return index_buffer_.get();
}
}  // namespace dw
