/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "renderer/Texture.h"
#include "renderer/rhi/RHIRenderer.h"

namespace dw {
class DW_API FrameBuffer : public Object {
public:
    DW_OBJECT(FrameBuffer);

    FrameBuffer(Context* ctx, const Vec2i& size, rhi::TextureFormat format);
    FrameBuffer(Context* ctx, Vector<SharedPtr<Texture>> textures);
    ~FrameBuffer();

    rhi::FrameBufferHandle internalHandle() const;

private:
    rhi::FrameBufferHandle handle_;
};
}  // namespace dw
