/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "Common.h"
#include "core/math/Colour.h"
#include "scene/Component.h"
#include "scene/System.h"

using namespace dw;

struct Projectile : public Component {
    Vec3 velocity;
    float damage;
    Colour colour;
};

class ProjectileSystem : public System {
public:
    DW_OBJECT(ProjectileSystem);

    explicit ProjectileSystem(Context* context);

    void processEntity(Entity& entity, float dt) override;
};