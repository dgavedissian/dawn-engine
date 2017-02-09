/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "resource/Resource.h"

namespace dw {

class DW_API ShaderProgram : public Resource {
public:
    DW_OBJECT(ShaderProgram);

    ShaderProgram(Context* context);
    ~ShaderProgram();

    bool beginLoad(InputStream& src) override;
    void endLoad() override;

    bgfx::ShaderHandle getHandle() const;

private:
    byte* mSrcData;
    u32 mSrcLen;
    bgfx::ShaderHandle mHandle;
};
}
