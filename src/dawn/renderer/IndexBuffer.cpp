/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/IndexBuffer.h"
#include "renderer/Renderer.h"

namespace dw {
IndexBuffer::IndexBuffer(Context* ctx, Memory data, rhi::IndexBufferType type,
                         rhi::BufferUsage usage)
    : Object{ctx}, type_{type} {
    if (type == rhi::IndexBufferType::U16) {
        index_count_ = data.size() / sizeof(u16);
    } else if (type == rhi::IndexBufferType::U32) {
        index_count_ = data.size() / sizeof(u32);
    } else {
        assert(false);
    }
    handle_ = context_->module<Renderer>()->rhi()->createIndexBuffer(std::move(data), type, usage);
}

IndexBuffer::~IndexBuffer() {
    context_->module<Renderer>()->rhi()->deleteIndexBuffer(handle_);
}

void IndexBuffer::update(Memory data, uint offset) {
    if (type_ == rhi::IndexBufferType::U16) {
        index_count_ = data.size() / sizeof(u16);
    } else if (type_ == rhi::IndexBufferType::U32) {
        index_count_ = data.size() / sizeof(u32);
    } else {
        assert(false);
    };
    context_->module<Renderer>()->rhi()->updateIndexBuffer(handle_, std::move(data), offset);
}

void IndexBuffer::bind(rhi::RHIRenderer* r) {
    r->setIndexBuffer(handle_);
}

rhi::IndexBufferHandle IndexBuffer::internalHandle() const {
    return handle_;
}

usize IndexBuffer::indexCount() const {
    return index_count_;
}
}  // namespace dw
