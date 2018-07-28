/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "scene/SLinearMotion.h"

namespace dw {
SLinearMotion::SLinearMotion(Context* ctx) : System(ctx) {
    supportsComponents<CLinearMotion, CTransform>();
}

void SLinearMotion::processEntity(Entity& e, float dt) {
    e.transform()->largeNode().position += e.component<CLinearMotion>()->velocity * dt;
}
}  // namespace dw
