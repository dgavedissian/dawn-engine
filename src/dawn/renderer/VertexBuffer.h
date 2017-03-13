/*
* Dawn Engine
* Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
*/
#pragma once

namespace dw {
class DW_API VertexBuffer : public Object {
public:
    DW_OBJECT(VertexBuffer);

    VertexBuffer(Context* context);
    ~VertexBuffer();

    bgfx::VertexBufferHandle internalHandle() const;

private:
    bgfx::VertexBufferHandle handle_;
};
}