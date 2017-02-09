/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/ShaderProgram.h"

namespace dw {

class Material : public Resource {
public:
    DW_OBJECT(Material);

    Material(Context* context, ShaderProgram* vs, ShaderProgram* ps);
    ~Material();

    bool beginLoad(InputStream& src) override;
    void endLoad() override;

    template <typename T> void setUniform(const String& name, const T& value) {
    }

private:
    Option<bgfx::UniformHandle> getUniformHandle(const String& name, bgfx::UniformType::Enum type,
                                                 int count);

    HashMap<String, Pair<bgfx::UniformHandle, bgfx::UniformType::Enum>> mUniformHandleTable;

    ShaderProgram* mVertexShader;
    ShaderProgram* mFragmentShader;

    bgfx::ProgramHandle mHandle;
};

template <> inline void Material::setUniform<float>(const String& name, const float& value) {
    Option<bgfx::UniformHandle> handle = getUniformHandle(name, bgfx::UniformType::Vec4, 1);
    if (handle.isSet()) {
        Vec4 value_v4(value, 0.0f, 0.0f, 0.0f);
        bgfx::setUniform(handle.get(), &value_v4);
    }
}

template <> inline void Material::setUniform<Vec2>(const String& name, const Vec2& value) {
    Option<bgfx::UniformHandle> handle = getUniformHandle(name, bgfx::UniformType::Vec4, 1);
    if (handle.isSet()) {
        Vec4 value_v4(value.x, value.y, 0.0f, 0.0f);
        bgfx::setUniform(handle.get(), &value_v4);
    }
}

template <> inline void Material::setUniform<Vec3>(const String& name, const Vec3& value) {
    Option<bgfx::UniformHandle> handle = getUniformHandle(name, bgfx::UniformType::Vec4, 1);
    if (handle.isSet()) {
        Vec4 value_v4(value, 0.0f);
        bgfx::setUniform(handle.get(), &value_v4);
    }
}

template <> inline void Material::setUniform<Vec4>(const String& name, const Vec4& value) {
    Option<bgfx::UniformHandle> handle = getUniformHandle(name, bgfx::UniformType::Vec4, 1);
    if (handle.isSet()) {
        bgfx::setUniform(handle.get(), &value);
    }
}
}
