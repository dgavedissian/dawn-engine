/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/IndexBuffer.h"

namespace dw {
GLIndexBuffer::GLIndexBuffer(Context* context) : Object(context) {
}

GLIndexBuffer::~GLIndexBuffer() {
}

// bgfx::IndexBufferHandle GLIndexBuffer::internalHandle() const {
//    return handle_;
//}
}  // namespace dw
