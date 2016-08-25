/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "resource/Resource.h"

namespace dw {

class Shader : public Resource {
public:
    DW_OBJECT(Shader);

    Shader(Context* context);
    ~Shader();

    bool beginLoad(InputStream& src) override;
    void endLoad() override;

    bgfx::ShaderHandle getHandle() const;

private:
    byte* mSrcData;
    u32 mSrcLen;
    bgfx::ShaderHandle mHandle;
};
}