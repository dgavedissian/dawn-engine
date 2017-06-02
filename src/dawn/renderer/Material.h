/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "core/Option.h"
#include "math/Defs.h"
#include "renderer/ShaderProgram.h"
#include "renderer/Texture.h"

namespace dw {

class DW_API Material : public Resource {
public:
    DW_OBJECT(Material);

    Material(Context* context, SharedPtr<ShaderProgram> vs, SharedPtr<ShaderProgram> fs);
    ~Material();

    bool beginLoad(InputStream& src) override;
    void endLoad() override;

    void setTextureUnit(SharedPtr<Texture> texture, uint unit = 0);

    template <typename T> void setUniform(const String& name, const T& value) {
    }

    bgfx::ProgramHandle internalHandle() const;

private:
    Option<bgfx::UniformHandle> uniformHandle(const String& name, bgfx::UniformType::Enum type,
                                              int count);

    HashMap<String, Pair<bgfx::UniformHandle, bgfx::UniformType::Enum>> uniform_handle_table_;

    SharedPtr<ShaderProgram> vertex_shader_;
    SharedPtr<ShaderProgram> fragment_shader_;
    Array<SharedPtr<Texture>, 8> texture_units_;

    bgfx::ProgramHandle handle_;
};

// Note: https://github.com/bkaradzic/bgfx/issues/653
// bgfx maps to hardware uniform types only. OpenGL and D3D map float/vec2/vec3 to a padded vec4
// type, for example.

template <> inline void Material::setUniform<float>(const String& name, const float& value) {
    Option<bgfx::UniformHandle> handle = uniformHandle(name, bgfx::UniformType::Vec4, 1);
    if (handle.isPresent()) {
        Vec4 value_v4(value, 0.0f, 0.0f, 0.0f);
        bgfx::setUniform(handle.get(), &value_v4);
    }
}

template <> inline void Material::setUniform<Vec2>(const String& name, const Vec2& value) {
    Option<bgfx::UniformHandle> handle = uniformHandle(name, bgfx::UniformType::Vec4, 1);
    if (handle.isPresent()) {
        Vec4 value_v4(value.x, value.y, 0.0f, 0.0f);
        bgfx::setUniform(handle.get(), &value_v4);
    }
}

template <> inline void Material::setUniform<Vec3>(const String& name, const Vec3& value) {
    Option<bgfx::UniformHandle> handle = uniformHandle(name, bgfx::UniformType::Vec4, 1);
    if (handle.isPresent()) {
        Vec4 value_v4(value, 0.0f);
        bgfx::setUniform(handle.get(), &value_v4);
    }
}

template <> inline void Material::setUniform<Vec4>(const String& name, const Vec4& value) {
    Option<bgfx::UniformHandle> handle = uniformHandle(name, bgfx::UniformType::Vec4, 1);
    if (handle.isPresent()) {
        bgfx::setUniform(handle.get(), &value);
    }
}
}  // namespace dw
