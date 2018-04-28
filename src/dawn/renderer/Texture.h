/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "resource/Resource.h"
#include "renderer/Renderer.h"

namespace dw {
class DW_API Texture : public Resource {
public:
    DW_OBJECT(Texture);

    Texture(Context* context);
    ~Texture();

    bool beginLoad(const String& asset_name, InputStream& src) override;
    void endLoad() override;

    rhi::TextureHandle internalHandle() const;

private:
    rhi::TextureHandle handle_;
};
}  // namespace dw
