/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "renderer/rhi/RHIRenderer.h"

namespace dw {
class DW_API VertexBuffer : public Object {
public:
    DW_OBJECT(VertexBuffer);

    VertexBuffer(Context* context, Memory data, usize vertex_count, const rhi::VertexDecl& decl,
                 rhi::BufferUsage usage = rhi::BufferUsage::Static);
    ~VertexBuffer();

    // Will resize.
    void update(Memory data, usize vertex_count, usize offset);

    void bind(rhi::RHIRenderer* r);

    rhi::VertexBufferHandle internalHandle() const;
    usize vertexCount() const;

private:
    rhi::VertexBufferHandle handle_;
    usize vertex_count_;
};
}  // namespace dw
