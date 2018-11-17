/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "resource/Resource.h"
#include "renderer/rhi/RHIRenderer.h"

namespace dw {
class DW_API Texture : public Resource {
public:
    DW_OBJECT(Texture);

    Texture(Context* context);
    ~Texture();

    static SharedPtr<Texture> createTexture2D(Context* ctx, const Vec2i& size,
                                              rhi::TextureFormat format, Memory data = {});

    // Resource.
    Result<None> beginLoad(const String& asset_name, InputStream& src) override;

    rhi::TextureHandle internalHandle() const;

private:
    rhi::TextureHandle handle_;
};
}  // namespace dw
