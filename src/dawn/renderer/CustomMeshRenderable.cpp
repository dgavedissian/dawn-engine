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

void CustomMeshRenderable::draw(Renderer* renderer, uint view, const Mat4& model_matrix,
                                const Mat4& view_projection_matrix) {
    u32 vertex_count = index_buffer_ ? index_buffer_->indexCount() : vertex_buffer_->vertexCount();
    renderer->setVertexBuffer(vertex_buffer_->internalHandle());
    if (index_buffer_) {
        renderer->setIndexBuffer(index_buffer_->internalHandle());
    }
    // TODO: Do this in the material class via a "bind" method.
    // TODO: Move this common "render vertex/index buffer + material" code somewhere.
    // TODO: Support unset material.
    auto program = material_->program();
    program->setUniform("model_matrix", model_matrix);
    program->setUniform("mvp_matrix", view_projection_matrix * model_matrix);
    program->prepareForRendering();
    renderer->submit(view, program->internalHandle(), vertex_count);
}

const VertexBuffer* CustomMeshRenderable::vertexBuffer() const {
    return vertex_buffer_.get();
}

const IndexBuffer* CustomMeshRenderable::indexBuffer() const {
    return index_buffer_.get();
}
}  // namespace dw
