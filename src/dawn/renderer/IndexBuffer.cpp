/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/IndexBuffer.h"
#include "renderer/Renderer.h"

namespace dw {
IndexBuffer::IndexBuffer(Context* context, const void* data, uint size, rhi::IndexBufferType type,
                         rhi::BufferUsage usage)
    : Object{context}, type_{type} {
    handle_ = context_->module<Renderer>()->rhi()->createIndexBuffer(data, size, type, usage);
    if (type == rhi::IndexBufferType::U16) {
        index_count_ = size / sizeof(u16);
    } else if (type == rhi::IndexBufferType::U32) {
        index_count_ = size / sizeof(u32);
    } else {
        assert(false);
    };
}

IndexBuffer::~IndexBuffer() {
    context_->module<Renderer>()->rhi()->deleteIndexBuffer(handle_);
}

void IndexBuffer::update(const void* data, uint size, uint offset) {
    if (type_ == rhi::IndexBufferType::U16) {
        index_count_ = size / sizeof(u16);
    } else if (type_ == rhi::IndexBufferType::U32) {
        index_count_ = size / sizeof(u32);
    } else {
        assert(false);
    };
    context_->module<Renderer>()->rhi()->updateIndexBuffer(handle_, data, size, offset);
}

rhi::IndexBufferHandle IndexBuffer::internalHandle() const {
    return handle_;
}

u32 IndexBuffer::indexCount() const {
    return index_count_;
}
}  // namespace dw
