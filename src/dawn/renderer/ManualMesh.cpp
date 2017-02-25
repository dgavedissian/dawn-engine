/*
* Dawn Engine
* Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
*/
#include "Common.h"
#include "renderer/ManualMesh.h"
#include "renderer/Material.h"

namespace dw {
ManualMesh::ManualMesh(Context* context, SharedPtr<VertexBuffer> vb, SharedPtr<IndexBuffer> ib)
    : Object(context), mVertexBuffer(vb), mIndexBuffer(ib) {
}

ManualMesh::~ManualMesh() {
}

void ManualMesh::draw(SharedPtr<Material> material) {
    bgfx::setVertexBuffer(mVertexBuffer->getInternalHandle());
    bgfx::setIndexBuffer(mIndexBuffer->getInternalHandle());
    bgfx::submit(0, material->getProgramInternalHandle());
}
}