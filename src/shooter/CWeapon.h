/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#pragma once

#include "Base.h"
#include "core/math/Colour.h"
#include "scene/Component.h"
#include "scene/SceneManager.h"
#include "scene/PhysicsScene.h"
#include "core/Timer.h"

#include "CProjectile.h"

using namespace dw;

struct CWeapon : public Component {
    CWeapon(int projectile_type, float projectile_speed, Colour projectile_colour,
            float cycle_time);

    int projectile_type;
    float projectile_speed;
    Colour projectile_colour;
    float cycle_time;

    bool firing;
    float cooldown;
};

class SWeapon : public EntitySystem<CTransform, CWeapon, CRigidBody> {
public:
    void process(SceneManager* scene_manager, float dt) override;
};
