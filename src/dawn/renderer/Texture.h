/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "resource/Resource.h"

namespace dw {
class DW_API Texture : public Resource {
public:
    DW_OBJECT(Texture);

    Texture(Context* context);
    ~Texture();

    bool beginLoad(InputStream& src) override;
    void endLoad() override;

    DEPRECATED bgfx::TextureHandle getTextureHandle() const;

private:
    bgfx::TextureHandle mTextureHandle;
};
}
