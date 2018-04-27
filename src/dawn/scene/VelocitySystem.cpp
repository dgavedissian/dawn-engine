/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "VelocitySystem.h"

namespace dw {
VelocitySystem::VelocitySystem(Context *ctx) : System(ctx) {
  supportsComponents<Velocity, Transform>();
}

void VelocitySystem::processEntity(Entity &e, float dt) {
  e.transform()->position() += e.component<Velocity>()->velocity * dt;
}
} // namespace dw