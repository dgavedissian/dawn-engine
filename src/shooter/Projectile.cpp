/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Projectile.h"

ProjectileSystem::ProjectileSystem(Context* context) : System(context) {
    supportsComponents<Projectile, Transform>();
}

void ProjectileSystem::processEntity(Entity& entity, float dt) {
    entity.component<Transform>()->position() += entity.component<Projectile>()->velocity * dt;
}
