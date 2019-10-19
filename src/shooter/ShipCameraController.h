/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "scene/Entity.h"

using namespace dw;

class ShipCameraController : public Object {
public:
    DW_OBJECT(ShipCameraController);

    explicit ShipCameraController(Context* ctx, const Vec3& offset);
    ~ShipCameraController() = default;

    void follow(const Entity* ship);
    void possess(Entity* camera);

    void update(float dt);

private:
    Entity* possessed_;
    const Entity* followed_;

    Vec3 offset_;
};
