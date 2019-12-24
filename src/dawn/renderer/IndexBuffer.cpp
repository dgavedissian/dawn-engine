/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "renderer/IndexBuffer.h"
#include "renderer/Renderer.h"

namespace dw {
IndexBuffer::IndexBuffer(Context* ctx, gfx::Memory data, gfx::IndexBufferType type,
                         gfx::BufferUsage usage)
    : Object{ctx}, type_{type} {
    if (type == gfx::IndexBufferType::U16) {
        index_count_ = data.size() / sizeof(u16);
    } else if (type == gfx::IndexBufferType::U32) {
        index_count_ = data.size() / sizeof(u32);
    } else {
        assert(false);
    }
    handle_ = context_->module<Renderer>()->rhi()->createIndexBuffer(std::move(data), type, usage);
}

IndexBuffer::IndexBuffer(Context* ctx, gfx::IndexBufferHandle handle, usize index_count,
                         gfx::IndexBufferType type)
    : Object{ctx}, handle_{handle}, index_count_{index_count}, type_{type} {
}

IndexBuffer::~IndexBuffer() {
    context_->module<Renderer>()->rhi()->deleteIndexBuffer(handle_);
}

void IndexBuffer::update(gfx::Memory data, uint offset) {
    if (type_ == gfx::IndexBufferType::U16) {
        index_count_ = data.size() / sizeof(u16);
    } else if (type_ == gfx::IndexBufferType::U32) {
        index_count_ = data.size() / sizeof(u32);
    } else {
        assert(false);
    };
    context_->module<Renderer>()->rhi()->updateIndexBuffer(handle_, std::move(data), offset);
}

void IndexBuffer::bind(gfx::Renderer* r) {
    r->setIndexBuffer(handle_);
}

gfx::IndexBufferHandle IndexBuffer::internalHandle() const {
    return handle_;
}

usize IndexBuffer::indexCount() const {
    return index_count_;
}

}  // namespace dw
