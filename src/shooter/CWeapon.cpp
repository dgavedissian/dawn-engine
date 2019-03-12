/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "CWeapon.h"
#include "scene/SceneManager.h"

CWeapon::CWeapon(int projectile_type, float projectile_speed, Colour projectile_colour,
                 float cycle_time)
    : projectile_type(projectile_type),
      projectile_speed(projectile_speed),
      projectile_colour(projectile_colour),
      cycle_time(cycle_time),
      firing(false),
      cooldown(0.0f) {
}

SWeapon::SWeapon(Context* ctx) : EntitySystem(ctx) {
    supportsComponents<CTransform, CWeapon, CRigidBody>();
}

void SWeapon::processEntity(Entity& entity, float dt) {
    auto& data = *entity.component<CWeapon>();

    if (data.firing) {
        // Has the cycle time elapsed?
        if (data.cooldown < M_EPSILON) {
            Vec3 ship_velocity = entity.component<CRigidBody>()->_rigidBody()->getLinearVelocity();

            // Fire projectile.
            Mat4 world_transform = entity.component<CTransform>()->node->deriveWorldModelMatrix();
            Vec3 direction = world_transform.TransformDir(-Vec3::unitZ).Normalized();
            Vec3 position = world_transform.TranslatePart();
            entity.sceneManager()->system<SProjectile>()->createNewProjectile(
                data.projectile_type, position, direction,
                direction * data.projectile_speed + ship_velocity, data.projectile_colour);

            // Reset cooldown.
            data.cooldown = data.cycle_time;
        }
    }

    // Cool down.
    data.cooldown = max(data.cooldown - dt, 0.0f);
}
