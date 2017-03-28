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

RenderTask ManualMesh::draw(const Mat4& modelMatrix) {
    RenderTask task;
    task.type = RenderTaskType::Primitive;
    task.primitive = {modelMatrix, vertex_buffer_->internalHandle(),
                      index_buffer_->internalHandle(), material_->internalHandle()};
    return task;
}
}