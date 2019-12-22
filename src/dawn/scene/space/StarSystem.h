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
class DW_API StarSystem : public Object {
public:
    DW_OBJECT(StarSystem);

    StarSystem(Context* ctx, SystemNode& root_system_node);
    ~StarSystem() override = default;

    SystemBody& root() const;
    SystemBody& addPlanet(const PlanetDesc& desc, SystemBody& parent, UniquePtr<Orbit> orbit);
    SystemBody& addStar(const StarDesc& desc, SystemBody& parent, UniquePtr<Orbit> orbit);

private:
    UniquePtr<SystemBody> root_body_;
};
}