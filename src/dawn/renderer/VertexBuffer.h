/*
* Dawn Engine
* Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
*/
#pragma once

namespace dw {
namespace renderer {
class DW_API VertexBuffer : public Object {
public:
    DW_OBJECT(VertexBuffer);

    VertexBuffer(Context* context);
    ~VertexBuffer();

    bgfx::VertexBufferHandle getInternalHandle();

private:
    bgfx::VertexBufferHandle mHandle;
};
}
}
