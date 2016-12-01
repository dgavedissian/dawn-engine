/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "resource/Resource.h"

namespace dw {

class Texture : public Resource {
public:
    DW_OBJECT(Texture);

    Texture(Context* context);
    ~Texture();

    bool beginLoad(InputStream& src) override;
    void endLoad() override;

    bgfx::TextureHandle getTextureHandle() const;

private:
    bgfx::TextureHandle mTextureHandle;

};

}