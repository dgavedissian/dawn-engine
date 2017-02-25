/*
* Dawn Engine
* Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
*/
#include "Common.h"
#include "renderer/ManualMesh.h"
#include "renderer/Material.h"

namespace dw {
ManualMesh::ManualMesh(Context* context, SharedPtr<VertexBuffer> vb, SharedPtr<IndexBuffer> ib)
    : Renderable{context}, vertex_buffer_{vb}, index_buffer_{ib} {
}

ManualMesh::~ManualMesh() {
}

void ManualMesh::Draw() {
    bgfx::setVertexBuffer(vertex_buffer_->getInternalHandle());
    bgfx::setIndexBuffer(index_buffer_->getInternalHandle());
    bgfx::submit(0, GetMaterial()->getProgramInternalHandle());
}
}