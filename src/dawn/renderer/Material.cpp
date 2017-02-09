/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Material.h"

namespace dw {

Material::Material(Context* context, ShaderProgram* vs, ShaderProgram* fs)
    : Resource(context), mVertexShader(vs), mFragmentShader(fs) {
    mHandle = bgfx::createProgram(vs->getHandle(), fs->getHandle());
    // TODO(David): error checking
}

Material::~Material() {
    bgfx::destroyProgram(mHandle);
}

bool Material::beginLoad(InputStream& src) {
    getLog().error("Material loading unimplemented");
    return false;
}

void Material::endLoad() {
}

Option<bgfx::UniformHandle> Material::getUniformHandle(const String& name,
                                                       bgfx::UniformType::Enum type, int count) {
    auto it = mUniformHandleTable.find(name);
    if (it != mUniformHandleTable.end()) {
        if (type == (*it).second.second) {
            return (*it).second.first;
        } else {
            getLog().error("Unable to set uniform '%s', mismatched type %s != %s", name, type,
                           (*it).second.second);
            return Option<bgfx::UniformHandle>();
        }
    } else {
        return Option<bgfx::UniformHandle>();
    }
}
}
