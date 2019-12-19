/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "core/math/Defs.h"
#include "renderer/Shader.h"
#include "renderer/Texture.h"
#include <dawn-gfx/Renderer.h>

namespace dw {
class DW_API Program : public Resource {
public:
    DW_OBJECT(Program);

    Program(Context* ctx, SharedPtr<VertexShader> vs, SharedPtr<FragmentShader> fs);
    ~Program() override;

    // Resource.
    Result<void> beginLoad(const String& asset_name, InputStream& src) override;

    void setTextureUnit(SharedPtr<Texture> texture, uint unit = 0);

    template <typename T> void setUniform(const String& name, const T& value) {
        uniforms_[name] = value;
    }

    void applyRendererState();

    gfx::ProgramHandle internalHandle() const;

private:
    gfx::Renderer* r;
    SharedPtr<Shader> vertex_shader_;
    SharedPtr<Shader> fragment_shader_;
    Array<SharedPtr<Texture>, 8> texture_units_;
    HashMap<String, gfx::UniformData> uniforms_;

    gfx::ProgramHandle handle_;
};
}  // namespace dw
