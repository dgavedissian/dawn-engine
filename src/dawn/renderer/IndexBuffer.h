/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once
#include <dawn-gfx/Renderer.h>

namespace dw {
class DW_API IndexBuffer : public Object {
public:
    DW_OBJECT(IndexBuffer);

    IndexBuffer(Context* ctx, gfx::Memory data, gfx::IndexBufferType type,
                gfx::BufferUsage usage = gfx::BufferUsage::Static);
    IndexBuffer(Context* ctx, gfx::IndexBufferHandle handle, usize index_count, gfx::IndexBufferType type);
    ~IndexBuffer() override;

    // Will resize.
    void update(gfx::Memory data, uint offset);

    void bind(gfx::Renderer* r);

    gfx::IndexBufferHandle internalHandle() const;
    usize indexCount() const;

private:
    gfx::IndexBufferHandle handle_;
    usize index_count_;
    gfx::IndexBufferType type_;
};
}  // namespace dw
