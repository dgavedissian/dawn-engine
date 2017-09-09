/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Material.h"

namespace dw {

Material::Material(Context* context, SharedPtr<Program> program)
    : Object{context}, program_{program} {
    //    handle_ = bgfx::createProgram(vs->internalHandle(), fs->internalHandle());

    // TODO(David): error checking
}

Material::~Material() {
}

void Material::setTextureUnit(SharedPtr<Texture> texture, uint unit) {
    program_->setTextureUnit(texture, unit);
}

Program* Material::program() {
    return program_.get();
}
}  // namespace dw
