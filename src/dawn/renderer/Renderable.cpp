/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Renderable.h"

namespace dw {
Renderable::Renderable() {
}

Renderable::~Renderable() {
}

void Renderable::setMaterial(SharedPtr<Material> material) {
    material_ = material;
}

Material* Renderable::material() const {
    return material_.get();
}
}  // namespace dw
