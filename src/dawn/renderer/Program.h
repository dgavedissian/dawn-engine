/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "core/Option.h"
#include "math/Defs.h"
#include "renderer/api/GL.h"
#include "renderer/Shader.h"
#include "renderer/Texture.h"

namespace dw {
class DW_API Program : public Resource {
public:
    DW_OBJECT(Program);

    Program(Context* context, SharedPtr<VertexShader> vs, SharedPtr<FragmentShader> fs);
    ~Program();

    bool beginLoad(InputStream& src) override;
    void endLoad() override;

    void setTextureUnit(SharedPtr<Texture> texture, uint unit = 0);

    template <typename T> void setUniform(const String& name, const T& value) {
        uniforms_[name] = value;
    }

    void prepareForRendering();

    ProgramHandle internalHandle() const;

private:
    Renderer* r;
    SharedPtr<Shader> vertex_shader_;
    SharedPtr<Shader> fragment_shader_;
    Array<SharedPtr<Texture>, 8> texture_units_;
    HashMap<String, UniformData> uniforms_;

    ProgramHandle handle_;
};
}  // namespace dw
