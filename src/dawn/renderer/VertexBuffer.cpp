/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "renderer/VertexBuffer.h"
#include "renderer/Renderer.h"

namespace dw {
VertexBuffer::VertexBuffer(Context* context, Memory data, usize vertex_count,
                           const rhi::VertexDecl& decl, rhi::BufferUsage usage)
    : Object{context}, vertex_count_{vertex_count} {
    handle_ = context->module<Renderer>()->rhi()->createVertexBuffer(std::move(data), decl, usage);
}

VertexBuffer::~VertexBuffer() {
    context_->module<Renderer>()->rhi()->deleteVertexBuffer(handle_);
}

void VertexBuffer::update(Memory data, usize vertex_count, usize offset) {
    vertex_count_ = vertex_count;
    context_->module<Renderer>()->rhi()->updateVertexBuffer(handle_, std::move(data), offset);
}

void VertexBuffer::bind(rhi::RHIRenderer* r) {
    r->setVertexBuffer(handle_);
}

rhi::VertexBufferHandle VertexBuffer::internalHandle() const {
    return handle_;
}

usize VertexBuffer::vertexCount() const {
    return vertex_count_;
}
}  // namespace dw
