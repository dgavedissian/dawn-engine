/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Material.h"

namespace dw {

Material::Material(Context* context, SharedPtr<ShaderProgram> vs, SharedPtr<ShaderProgram> fs)
    : Resource(context), mVertexShader(vs), mFragmentShader(fs) {
    mHandle = bgfx::createProgram(vs->getInternalHandle(), fs->getInternalHandle());
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

bgfx::ProgramHandle Material::getProgramInternalHandle() {
    return mHandle;
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
