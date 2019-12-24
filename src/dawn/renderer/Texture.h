/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "resource/Resource.h"
#include <dawn-gfx/Renderer.h>

namespace dw {
class DW_API Texture : public Resource {
public:
    DW_OBJECT(Texture);

    explicit Texture(Context* context);
    ~Texture() override;

    static SharedPtr<Texture> createTexture2D(Context* ctx, const Vec2i& size,
                                              gfx::TextureFormat format, gfx::Memory data = {});

    // Resource.
    Result<void> beginLoad(const String& asset_name, InputStream& src) override;

    gfx::TextureHandle internalHandle() const;

private:
    gfx::TextureHandle handle_;
};
}  // namespace dw
