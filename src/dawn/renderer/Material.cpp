/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Material.h"

namespace dw {

Material::Material(Context* context, SharedPtr<ShaderProgram> vs, SharedPtr<ShaderProgram> fs)
    : Resource(context), vertex_shader_(vs), fragment_shader_(fs) {
    handle_ = bgfx::createProgram(vs->internalHandle(), fs->internalHandle());
    // TODO(David): error checking
}

Material::~Material() {
    bgfx::destroyProgram(handle_);
}

bool Material::beginLoad(InputStream& src) {
    log().error("Material loading unimplemented");
    return false;
}

void Material::endLoad() {
}

bgfx::ProgramHandle Material::internalHandle() {
    return handle_;
}

Option<bgfx::UniformHandle> Material::uniformHandle(const String& name,
                                                    bgfx::UniformType::Enum type, int count) {
    auto it = uniform_handle_table_.find(name);
    if (it != uniform_handle_table_.end()) {
        if (type == (*it).second.second) {
            return (*it).second.first;
        } else {
            log().error("Unable to set uniform '%s', mismatched type %s != %s", name, type,
                        (*it).second.second);
            return Option<bgfx::UniformHandle>();
        }
    } else {
        return Option<bgfx::UniformHandle>();
    }
}
}
