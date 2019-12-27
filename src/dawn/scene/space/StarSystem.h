/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "scene/space/Planet.h"
#include "scene/space/Star.h"
#include "scene/space/Orbit.h"
#include "scene/space/SystemBody.h"
#include "scene/Entity.h"

namespace dw {
class Frame;
class DW_API StarSystem : public Object {
public:
    DW_OBJECT(StarSystem);

    StarSystem(Context* ctx, SystemNode& root_system_node);
    ~StarSystem() override = default;

    SystemBody& root() const;
    SystemBody& addPlanet(const PlanetDesc& desc, SystemBody& parent, UniquePtr<Orbit> orbit);
    SystemBody& addStar(const StarDesc& desc, SystemBody& parent, UniquePtr<Orbit> orbit);

    void update(float dt, Frame& frame, const Vec3& camera_position);
    void updatePosition(double time);

    const Vector<SystemBody*>& getSystemBodies() const;
    const Vector<Star*>& getStars() const;

private:
    UniquePtr<SystemBody> root_body_;
    Vector<SystemBody*> system_bodies_;
    Vector<Star*> stars_;
};
}