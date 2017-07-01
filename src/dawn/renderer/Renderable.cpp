/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Renderable.h"
#include "renderer/Renderer.h"

namespace dw {

Renderable::Renderable(Context* context) : Object(context) {
}

Renderable::~Renderable() {
}

void Renderable::setMaterial(SharedPtr<Program> material) {
    material_ = material;
}

Program* Renderable::material() const {
    return material_.get();
}
}  // namespace dw
