/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/GLVertexBuffer.h"

namespace dw {
GLVertexBuffer::GLVertexBuffer(Context* context) : Object(context) {
}

GLVertexBuffer::~GLVertexBuffer() {
}

// bgfx::VertexBufferHandle GLVertexBuffer::internalHandle() const {
//    return handle_;
//}
}  // namespace dw
