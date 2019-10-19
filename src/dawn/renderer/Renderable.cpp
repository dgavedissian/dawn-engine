/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
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
