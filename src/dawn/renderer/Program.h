/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "core/math/Defs.h"
#include "renderer/Shader.h"
#include "renderer/Texture.h"
#include "renderer/rhi/Renderer.h"

namespace dw {
class DW_API Program : public Resource {
public:
    DW_OBJECT(Program);

    Program(Context* ctx, SharedPtr<VertexShader> vs, SharedPtr<FragmentShader> fs);
    ~Program();

    bool beginLoad(const String& asset_name, InputStream& src) override;
    void endLoad() override;

    void setTextureUnit(SharedPtr<Texture> texture, uint unit = 0);

    template <typename T> void setUniform(const String& name, const T& value) {
        uniforms_[name] = value;
    }

    void applyRendererState();

    rhi::ProgramHandle internalHandle() const;

private:
    rhi::Renderer* r;
    SharedPtr<Shader> vertex_shader_;
    SharedPtr<Shader> fragment_shader_;
    Array<SharedPtr<Texture>, 8> texture_units_;
    HashMap<String, rhi::UniformData> uniforms_;

    rhi::ProgramHandle handle_;
};
}  // namespace dw
