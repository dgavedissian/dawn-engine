/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "Common.h"
#include "core/math/Colour.h"
#include "scene/Component.h"
#include "scene/EntitySystem.h"
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

class SWeapon : public EntitySystem {
public:
    DW_OBJECT(SWeapon);

    explicit SWeapon(Context* context);

    void processEntity(Entity& entity, float dt) override;
};
