/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/CustomMesh.h"
#include "renderer/Program.h"

namespace dw {
CustomMesh::CustomMesh(Context* context, SharedPtr<VertexBuffer> vb, SharedPtr<GLIndexBuffer> ib)
    : Renderable{context}, vertex_buffer_{vb}, index_buffer_{ib} {
}

CustomMesh::~CustomMesh() {
}

RenderTask CustomMesh::draw(const Mat4& modelMatrix) {
    RenderTask task;
    task.type = RenderTaskType::Primitive;
    //    task.primitive = {modelMatrix, vertex_buffer_->internalHandle(),
    //                      index_buffer_->internalHandle(), material_->internalHandle()};
    return task;
}
}  // namespace dw