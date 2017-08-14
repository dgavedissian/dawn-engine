/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Renderer.h"

namespace dw {
class DW_API IndexBuffer : public Object {
public:
    DW_OBJECT(IndexBuffer);

    IndexBuffer(Context* context, const void* data, uint size, IndexBufferType type);
    ~IndexBuffer();

    IndexBufferHandle internalHandle() const;
    u32 indexCount() const;

private:
    IndexBufferHandle handle_;
    u32 index_count_;
};
}  // namespace dw
