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
    for (auto uniform : uniform_handle_table_) {
        bgfx::destroyUniform(uniform.second.first);
    }
    bgfx::destroyProgram(handle_);
}

bool Material::beginLoad(InputStream&) {
    log().error("Material loading unimplemented");
    return false;
}

void Material::endLoad() {
}

void Material::setTextureUnit(SharedPtr<Texture> /*texture*/, uint /*unit*/) {
}

bgfx::ProgramHandle Material::internalHandle() const {
    return handle_;
}

Option<bgfx::UniformHandle> Material::uniformHandle(const String& name,
                                                    bgfx::UniformType::Enum type, int count) {
    auto it = uniform_handle_table_.find(name);
    if (it != uniform_handle_table_.end()) {
        if (type == (*it).second.second) {
            return (*it).second.first;
        }
        auto uniform_type = [](bgfx::UniformType::Enum type) -> String {
            switch (type) {
            case bgfx::UniformType::Int1:
                return "Int1";
            case bgfx::UniformType::Vec4:
                return "Vec4";
            case bgfx::UniformType::Mat3:
                return "Mat3";
            case bgfx::UniformType::Mat4:
                return "Mat4";
            default:
                return tfm::format("UNKNOWN(%s)", type);
            }
        };
	    log().error("Unable to obtain uniform '%s', mismatched type: %s != %s.", name, uniform_type(type),
            uniform_type((*it).second.second));
	    return Option<bgfx::UniformHandle>();
    }
    auto handle = bgfx::createUniform(name.c_str(), type, count);
    if (handle.idx == bgfx::invalidHandle) {
        log().error("Unable to obtain uniform '%s'.", name);
        return Option<bgfx::UniformHandle>();
    }
    uniform_handle_table_.emplace(name, makePair(handle, type));
    return Option<bgfx::UniformHandle>(handle);
}
}
