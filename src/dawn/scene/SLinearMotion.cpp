/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "scene/SLinearMotion.h"

namespace dw {
SLinearMotion::SLinearMotion(Context* ctx) : EntitySystem(ctx) {
    supportsComponents<CLinearMotion, CTransform>();
}

void SLinearMotion::processEntity(Entity& e, float dt) {
    e.transform()->position += e.component<CLinearMotion>()->velocity * dt;
}
}  // namespace dw
