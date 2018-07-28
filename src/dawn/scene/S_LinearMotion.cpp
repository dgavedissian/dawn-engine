/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "scene/S_LinearMotion.h"

namespace dw {
S_LinearMotion::S_LinearMotion(Context* ctx) : System(ctx) {
    supportsComponents<C_LinearMotion, C_Transform>();
}

void S_LinearMotion::processEntity(Entity& e, float dt) {
    e.transform()->position += e.component<C_LinearMotion>()->velocity * dt;
}
}  // namespace dw
