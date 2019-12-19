/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once
#include <dawn-gfx/Renderer.h>

namespace dw {
class DW_API VertexBuffer : public Object {
public:
    DW_OBJECT(VertexBuffer);

    VertexBuffer(Context* context, gfx::Memory data, usize vertex_count, const gfx::VertexDecl& decl,
                 gfx::BufferUsage usage = gfx::BufferUsage::Static);
    VertexBuffer(Context* context, gfx::VertexBufferHandle handle, usize vertex_count);
    ~VertexBuffer() override;

    // Will resize.
    void update(gfx::Memory data, usize vertex_count, usize offset);

    void bind(gfx::Renderer* r);

    gfx::VertexBufferHandle internalHandle() const;
    usize vertexCount() const;

private:
    gfx::VertexBufferHandle handle_;
    usize vertex_count_;
};
}  // namespace dw
