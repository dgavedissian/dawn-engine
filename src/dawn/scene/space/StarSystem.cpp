/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Core.h"
#include "StarSystem.h"
#include "resource/ResourceCache.h"
#include "renderer/SceneGraph.h"
#include "scene/space/Barycentre.h"

namespace dw {
StarSystem::StarSystem(Context* ctx, SystemNode& root_system_node) : Object{ctx} {
    root_body_ = makeUnique<Barycentre>(ctx, 0.0f, *root_system_node.newChild());
}

SystemBody& StarSystem::root() const {
    return *root_body_;
}

SystemBody& StarSystem::addPlanet(const PlanetDesc& desc, SystemBody& parent,
                                  UniquePtr<Orbit> orbit) {
    auto planet =
        makeUnique<Planet>(context(), *root_body_->getSystemNode().newChild(), *this, desc);
    system_bodies_.emplace_back(planet.get());
    return parent.addSatellite(std::move(planet), std::move(orbit));
}

SystemBody& StarSystem::addStar(const StarDesc& desc, SystemBody& parent, UniquePtr<Orbit> orbit) {
    auto star = makeUnique<Star>(context(), *root_body_->getSystemNode().newChild(), desc);
    system_bodies_.emplace_back(star.get());
    stars_.emplace_back(star.get());
    return parent.addSatellite(std::move(star), std::move(orbit));
}

void StarSystem::update(float dt, Frame& frame, const Vec3& camera_position) {
    root_body_->preRender(frame);
    root_body_->update(dt, frame, camera_position);
}

void StarSystem::updatePosition(double time) {
    root_body_->updatePosition(time);
}

const Vector<SystemBody*>& StarSystem::getSystemBodies() const {
    return system_bodies_;
}

const Vector<Star*>& StarSystem::getStars() const {
    return stars_;
}
}  // namespace dw