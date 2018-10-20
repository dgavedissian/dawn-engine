/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/VertexBuffer.h"
#include "renderer/Renderer.h"

namespace dw {
VertexBuffer::VertexBuffer(Context* context, Memory data, uint vertex_count,
                           const rhi::VertexDecl& decl, rhi::BufferUsage usage)
    : Object{context}, vertex_count_{vertex_count} {
    handle_ = context->module<Renderer>()->rhi()->createVertexBuffer(std::move(data), decl, usage);
}

VertexBuffer::~VertexBuffer() {
    context_->module<Renderer>()->rhi()->deleteVertexBuffer(handle_);
}

void VertexBuffer::update(Memory data, uint vertex_count, uint offset) {
    vertex_count_ = vertex_count;
    context_->module<Renderer>()->rhi()->updateVertexBuffer(handle_, std::move(data), offset);
}

void VertexBuffer::bind(rhi::Renderer* r) {
    r->setVertexBuffer(handle_);
}

rhi::VertexBufferHandle VertexBuffer::internalHandle() const {
    return handle_;
}

u32 VertexBuffer::vertexCount() const {
    return vertex_count_;
}
}  // namespace dw
