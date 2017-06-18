/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "resource/Resource.h"
#include "renderer/GL.h"

namespace dw {
class DW_API GLTexture : public Resource {
public:
    DW_OBJECT(GLTexture);

    GLTexture(Context* context);
    ~GLTexture();

    bool beginLoad(InputStream& src) override;
    void endLoad() override;

    //    bgfx::TextureHandle internalHandle() const;

private:
    //    bgfx::TextureHandle handle_;
};
}  // namespace dw
