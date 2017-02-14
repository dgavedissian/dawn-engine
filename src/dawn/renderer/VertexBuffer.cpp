/*
* Dawn Engine
* Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
*/
#include "Common.h"
#include "renderer/VertexBuffer.h"

namespace dw {
namespace renderer {

VertexBuffer::VertexBuffer(Context* context) : Object(context) {
}

VertexBuffer::~VertexBuffer() {
}

bgfx::VertexBufferHandle VertexBuffer::getInternalHandle() const {
    return mHandle;
}
}
}
