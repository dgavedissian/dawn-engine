/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "renderer/Shader.h"
#include "renderer/ShaderProgram.h"

namespace dw {

ShaderProgram::ShaderProgram(Context *context, Shader *vs, Shader *fs)
    : Object(context), mVertexShader(vs), mFragmentShader(fs) {
    mHandle = bgfx::createProgram(vs->getHandle(), fs->getHandle());
    // TODO(David): error checking
}

ShaderProgram::~ShaderProgram() {
    bgfx::destroyProgram(mHandle);
}

Option<bgfx::UniformHandle> ShaderProgram::getUniformHandle(const String &name,
                                                            bgfx::UniformType::Enum type,
                                                            int count) {
    auto it = mUniformHandleTable.find(name);
    if (it != mUniformHandleTable.end()) {
        if (type == (*it).second.second) {
            return (*it).second.first;
        } else {
            getLog().error << "Unable to set uniform '" << name << "', mismatched type " << type
                           << " != " << (*it).second.second;
            return Option<bgfx::UniformHandle>();
        }
    }
}
}