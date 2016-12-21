/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "renderer/Shader.h"

namespace dw {

Shader::Shader(Context *context, ShaderSource *vs, ShaderSource *fs)
    : Object(context), mVertexShader(vs), mFragmentShader(fs) {
    mHandle = bgfx::createProgram(vs->getHandle(), fs->getHandle());
    // TODO(David): error checking
}

Shader::~Shader() {
    bgfx::destroyProgram(mHandle);
}

Option<bgfx::UniformHandle> Shader::getUniformHandle(const String &name,
                                                     bgfx::UniformType::Enum type, int count) {
    auto it = mUniformHandleTable.find(name);
    if (it != mUniformHandleTable.end()) {
        if (type == (*it).second.second) {
            return (*it).second.first;
        } else {
            getLog().error("Unable to set uniform '%s', mismatched type %s != %s", name, type, (*it).second.second);
            return Option<bgfx::UniformHandle>();
        }
    } else {
        return Option<bgfx::UniformHandle>();
    }
}
}