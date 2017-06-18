/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "resource/Resource.h"
#include "renderer/GL.h"

namespace dw {
class DW_API GLShader : public Resource {
public:
    DW_OBJECT(GLShader);

    GLShader(Context* context);
    ~GLShader();

    bool beginLoad(InputStream& src) override;
    void endLoad() override;

    //    bgfx::ShaderHandle internalHandle() const;

private:
    byte* src_data_;
    u32 src_len_;
    //    bgfx::ShaderHandle handle_;
};
}  // namespace dw
