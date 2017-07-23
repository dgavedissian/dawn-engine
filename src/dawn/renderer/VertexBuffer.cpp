/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/VertexBuffer.h"

namespace dw {
VertexBuffer::VertexBuffer(Context* context, const void* data, uint size, uint vertex_count,
                           const VertexDecl& decl)
    : Object{context}, vertex_count_{vertex_count} {
    handle_ = context->subsystem<Renderer>()->createVertexBuffer(data, size, decl);
}

VertexBuffer::~VertexBuffer() {
    context_->subsystem<Renderer>()->deleteVertexBuffer(handle_);
}

VertexBufferHandle VertexBuffer::internalHandle() const {
    return handle_;
}

u32 VertexBuffer::vertexCount() const {
    return vertex_count_;
}
}  // namespace dw
