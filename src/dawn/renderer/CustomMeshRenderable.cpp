/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/CustomMeshRenderable.h"
#include "renderer/Program.h"

namespace dw {
CustomMeshRenderable::CustomMeshRenderable(Context* context, SharedPtr<VertexBuffer> vertex_buffer,
                                           SharedPtr<IndexBuffer> index_buffer)
    : Object{context}, vertex_buffer_{vertex_buffer}, index_buffer_{index_buffer} {
}

CustomMeshRenderable::~CustomMeshRenderable() {
}

void CustomMeshRenderable::draw(Renderer* renderer, uint view, Transform* camera,
                                const Mat4& model_matrix, const Mat4& view_projection_matrix) {
    u32 vertex_count = index_buffer_ ? index_buffer_->indexCount() : vertex_buffer_->vertexCount();
    renderer->setVertexBuffer(vertex_buffer_->internalHandle());
    if (index_buffer_) {
        renderer->setIndexBuffer(index_buffer_->internalHandle());
    }
    // TODO: Move this common "render vertex/index buffer + material" code somewhere to avoid
    // duplication with Mesh.
    // TODO: Support unset material.
    material_->applyRendererState(model_matrix, view_projection_matrix);
    renderer->submit(view, material_->program()->internalHandle(), vertex_count);
}

VertexBuffer* CustomMeshRenderable::vertexBuffer() const {
    return vertex_buffer_.get();
}

IndexBuffer* CustomMeshRenderable::indexBuffer() const {
    return index_buffer_.get();
}
}  // namespace dw
