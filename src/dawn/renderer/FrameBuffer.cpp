/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "renderer/FrameBuffer.h"
#include "renderer/Renderer.h"

namespace dw {
FrameBuffer::FrameBuffer(Context* ctx, const Vec2i& size, rhi::TextureFormat format)
    : FrameBuffer(ctx, {Texture::createTexture2D(ctx, size, format)}) {
}

FrameBuffer::FrameBuffer(Context* ctx, Vector<SharedPtr<Texture>> textures) : Object(ctx) {
    Vector<rhi::TextureHandle> texture_handles;
    std::transform(textures.begin(), textures.end(), std::back_inserter(texture_handles),
                   [](const SharedPtr<Texture>& texture) -> rhi::TextureHandle {
                       return texture->internalHandle();
                   });
    handle_ = ctx->module<Renderer>()->rhi()->createFrameBuffer(texture_handles);
}

FrameBuffer::~FrameBuffer() {
    if (handle_.isValid()) {
        module<Renderer>()->rhi()->deleteFrameBuffer(handle_);
    }
}

rhi::FrameBufferHandle FrameBuffer::internalHandle() const {
    return handle_;
}
}  // namespace dw
