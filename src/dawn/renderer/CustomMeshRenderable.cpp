/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/CustomMeshRenderable.h"
#include "renderer/Program.h"

namespace dw {
CustomMeshRenderable::CustomMeshRenderable(Context* context, SharedPtr<VertexBuffer> vb,
                                           SharedPtr<IndexBuffer> ib)
    : Renderable{context}, vertex_buffer_{vb}, index_buffer_{ib} {
}

CustomMeshRenderable::~CustomMeshRenderable() {
}

void CustomMeshRenderable::draw(uint view, const Mat4& modelMatrix) {
    Renderer* r = subsystem<Renderer>();
    u32 vertex_count = index_buffer_ ? index_buffer_->indexCount() : vertex_buffer_->vertexCount();
    r->setVertexBuffer(vertex_buffer_->internalHandle());
    if (index_buffer_) {
        r->setIndexBuffer(index_buffer_->internalHandle());
    }
    material_->program()->prepareForRendering();
    r->submit(view, material_->program()->internalHandle(), vertex_count);
}

const VertexBuffer* CustomMeshRenderable::vertexBuffer() const {
    return vertex_buffer_.get();
}

const IndexBuffer* CustomMeshRenderable::indexBuffer() const {
    return index_buffer_.get();
}
}  // namespace dw
