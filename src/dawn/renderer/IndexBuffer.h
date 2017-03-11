/*
* Dawn Engine
* Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
*/
#pragma once

namespace dw {
class DW_API IndexBuffer : public Object {
public:
    DW_OBJECT(IndexBuffer);

    IndexBuffer(Context* context);
    ~IndexBuffer();

    bgfx::IndexBufferHandle internalHandle() const;

private:
    bgfx::IndexBufferHandle handle_;
};
}
