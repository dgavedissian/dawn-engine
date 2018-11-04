/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "renderer/rhi/RHIRenderer.h"

namespace dw {
class DW_API IndexBuffer : public Object {
public:
    DW_OBJECT(IndexBuffer);

    IndexBuffer(Context* context, Memory data, rhi::IndexBufferType type,
                rhi::BufferUsage usage = rhi::BufferUsage::Static);
    ~IndexBuffer();

    // Will resize.
    void update(Memory data, uint offset);

    void bind(rhi::RHIRenderer* r);

    rhi::IndexBufferHandle internalHandle() const;
    u32 indexCount() const;

private:
    rhi::IndexBufferHandle handle_;
    rhi::IndexBufferType type_;
    u32 index_count_;
};
}  // namespace dw
