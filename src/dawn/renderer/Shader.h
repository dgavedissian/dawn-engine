/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "resource/Resource.h"
#include "renderer/Renderer.h"

namespace dw {
class DW_API Shader : public Resource {
public:
    DW_OBJECT(Shader);

    Shader(Context* context);
    ~Shader();

    bool beginLoad(InputStream& src) override;
    void endLoad() override;

    ShaderHandle internalHandle() const;

private:
    byte* src_data_;
    u32 src_len_;
    ShaderHandle handle_;
};
}  // namespace dw
