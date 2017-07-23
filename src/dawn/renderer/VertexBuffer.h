/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Renderer.h"

namespace dw {
class DW_API VertexBuffer : public Object {
public:
    DW_OBJECT(VertexBuffer);

    VertexBuffer(Context* context, const void* data, uint size, uint vertex_count,
                 const VertexDecl& decl);
    ~VertexBuffer();

    VertexBufferHandle internalHandle() const;
    u32 vertexCount() const;

private:
    VertexBufferHandle handle_;
    u32 vertex_count_;
};
}  // namespace dw
