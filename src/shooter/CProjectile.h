/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "Common.h"
#include "core/math/Colour.h"
#include "scene/Component.h"
#include "scene/System.h"

using namespace dw;

struct CProjectile : public Component {
    Vec3 velocity;
    float damage;
    Colour colour;
};

class SProjectile : public System {
public:
    DW_OBJECT(SProjectile);

    explicit SProjectile(Context* context);

    void processEntity(Entity& entity, float dt) override;
};