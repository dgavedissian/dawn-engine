/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

namespace dw {

class ShaderProgram : public Object {
public:
    DW_OBJECT(ShaderProgram);

    ShaderProgram(Context* context, Shader* vs, Shader* ps);
    ~ShaderProgram();

    template <class T> void setUniform(const String& name, const T& value) {
    }

private:
    Option<bgfx::UniformHandle> getUniformHandle(const String& name, bgfx::UniformType::Enum type,
                                                 int count);

    HashMap<String, Pair<bgfx::UniformHandle, bgfx::UniformType::Enum>> mUniformHandleTable;

    Shader* mVertexShader;
    Shader* mFragmentShader;

    bgfx::ProgramHandle mHandle;
};

template <> void ShaderProgram::setUniform<float>(const String& name, const float& value) {
    Option<bgfx::UniformHandle> handle = getUniformHandle(name, bgfx::UniformType::Vec4, 1);
    if (handle.isSet()) {
        Vec4 value_v4(value, 0.0f, 0.0f, 0.0f);
        bgfx::setUniform(handle.get(), &value_v4);
    }
}

template <> void ShaderProgram::setUniform<Vec2>(const String& name, const Vec2& value) {
    Option<bgfx::UniformHandle> handle = getUniformHandle(name, bgfx::UniformType::Vec4, 1);
    if (handle.isSet()) {
        Vec4 value_v4(value.x, value.y, 0.0f, 0.0f);
        bgfx::setUniform(handle.get(), &value_v4);
    }
}

template <> void ShaderProgram::setUniform<Vec3>(const String& name, const Vec3& value) {
    Option<bgfx::UniformHandle> handle = getUniformHandle(name, bgfx::UniformType::Vec4, 1);
    if (handle.isSet()) {
        Vec4 value_v4(value, 0.0f);
        bgfx::setUniform(handle.get(), &value_v4);
    }
}

template <> void ShaderProgram::setUniform<Vec4>(const String& name, const Vec4& value) {
    Option<bgfx::UniformHandle> handle = getUniformHandle(name, bgfx::UniformType::Vec4, 1);
    if (handle.isSet()) {
        bgfx::setUniform(handle.get(), &value);
    }
}
}
