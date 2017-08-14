/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/CustomMesh.h"
#include "renderer/Program.h"

namespace dw {
CustomMesh::CustomMesh(Context* context, SharedPtr<VertexBuffer> vb, SharedPtr<IndexBuffer> ib)
    : Renderable{context}, vertex_buffer_{vb}, index_buffer_{ib} {
}

CustomMesh::~CustomMesh() {
}

void CustomMesh::draw(Renderer* renderer, uint view, const Mat4& modelMatrix) {
    u32 vertex_count = index_buffer_ ? index_buffer_->indexCount() : vertex_buffer_->vertexCount();
    renderer->setVertexBuffer(vertex_buffer_->internalHandle());
    if (index_buffer_) {
        renderer->setIndexBuffer(index_buffer_->internalHandle());
    }
    renderer->submit(view, material_->internalHandle(), vertex_count);
}

const VertexBuffer *CustomMesh::vertexBuffer() const {
    return vertex_buffer_.get();
}

const IndexBuffer* CustomMesh::indexBuffer() const {
    return index_buffer_.get();
}
}  // namespace dw
