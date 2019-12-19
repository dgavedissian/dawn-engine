/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "renderer/Texture.h"
#include <dawn-gfx/Renderer.h>

namespace dw {
class DW_API FrameBuffer : public Object {
public:
    DW_OBJECT(FrameBuffer);

    FrameBuffer(Context* ctx, const Vec2i& size, gfx::TextureFormat format);
    FrameBuffer(Context* ctx, Vector<SharedPtr<Texture>> textures);
    ~FrameBuffer();

    gfx::FrameBufferHandle internalHandle() const;

private:
    gfx::FrameBufferHandle handle_;
};
}  // namespace dw
