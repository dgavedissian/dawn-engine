/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/GL.h"

namespace dw {
class DW_API GLVertexBuffer : public Object {
public:
    DW_OBJECT(GLVertexBuffer);

    GLVertexBuffer(Context* context);
    ~GLVertexBuffer();

    // bgfx::VertexBufferHandle internalHandle() const;

private:
    // bgfx::VertexBufferHandle handle_;
};
}  // namespace dw
