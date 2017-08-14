/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/IndexBuffer.h"

namespace dw {
IndexBuffer::IndexBuffer(Context* context, const void* data, uint size, IndexBufferType type)
    : Object{context} {
    handle_ = context_->subsystem<Renderer>()->createIndexBuffer(data, size, type);
    if (type == IndexBufferType::U16) {
        index_count_ = size / sizeof(u16);
    } else if (type == IndexBufferType::U32) {
        index_count_ = size / sizeof(u32);
    } else {
        assert(false);
    };
}

IndexBuffer::~IndexBuffer() {
    context_->subsystem<Renderer>()->deleteIndexBuffer(handle_);
}

IndexBufferHandle IndexBuffer::internalHandle() const {
    return handle_;
}

u32 IndexBuffer::indexCount() const {
    return index_count_;
}
}  // namespace dw
