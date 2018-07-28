/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "CProjectile.h"

SProjectile::SProjectile(Context* context) : System(context) {
    supportsComponents<CProjectile, CTransform>();
}

void SProjectile::processEntity(Entity& entity, float dt) {
    entity.component<CTransform>()->largeNode().position += entity.component<CProjectile>()->velocity * dt;
}
