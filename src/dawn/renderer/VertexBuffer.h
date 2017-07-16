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

    VertexBuffer(Context* context);
    ~VertexBuffer();

    VertexBufferHandle internalHandle() const;

private:
    VertexBufferHandle handle_;
};
}  // namespace dw
