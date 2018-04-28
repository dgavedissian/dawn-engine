/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Renderer.h"

namespace dw {
class DW_API VertexBuffer : public Object {
public:
    DW_OBJECT(VertexBuffer);

    VertexBuffer(Context* context, const void* data, uint size, uint vertex_count,
                 const rhi::VertexDecl& decl, rhi::BufferUsage usage = rhi::BufferUsage::Static);
    ~VertexBuffer();

    // Will resize.
    void update(const void* data, uint size, uint vertex_count, uint offset);

    rhi::VertexBufferHandle internalHandle() const;
    u32 vertexCount() const;

private:
    rhi::VertexBufferHandle handle_;
    u32 vertex_count_;
};
}  // namespace dw
