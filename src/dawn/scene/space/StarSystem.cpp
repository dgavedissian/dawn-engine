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
    root_body_ = makeUnique<Barycentre>(root_system_node);
}

SystemBody& StarSystem::root() const {
    return *root_body_;
}

SystemBody& StarSystem::addPlanet(const PlanetDesc& desc, SystemBody& parent,
                                  UniquePtr<Orbit> orbit) {
    auto rc = module<ResourceCache>();

    // Set up planet material.
    auto material = makeShared<Material>(
        context(), makeShared<Program>(context(), *rc->get<VertexShader>("base:space/planet.vs"),
                                       *rc->get<FragmentShader>("base:space/planet.fs")));
    material->setTexture(*rc->get<Texture>("base:space/planet.jpg"));
    material->setUniform("light_direction", Vec3{0.0f, 0.0f, 1.0f});
    material->setUniform("surface_sampler", 0);

    // Create renderable.
    auto renderable =
        CustomRenderable::Builder(context()).texcoords(true).normals(true).createSphere(
            desc.radius);
    renderable->setMaterial(material);

    // Create system node.
    auto& new_system_node =
        *parent.getSystemNode().newChild(SystemPosition::origin, Quat::identity);
    new_system_node.data.renderable = renderable;
    return parent.addSatellite(makeUnique<SystemBody>(new_system_node), std::move(orbit));
}

SystemBody& StarSystem::addStar(const StarDesc& desc, SystemBody& parent, UniquePtr<Orbit> orbit) {
    auto rc = module<ResourceCache>();

    // Set up planet material.
    auto material = makeShared<Material>(
        context(), makeShared<Program>(context(), *rc->get<VertexShader>("base:space/planet.vs"),
                                       *rc->get<FragmentShader>("base:space/planet.fs")));
    material->setTexture(*rc->get<Texture>("base:space/planet.jpg"));
    material->setUniform("light_direction", Vec3{0.0f, 0.0f, 1.0f});
    material->setUniform("surface_sampler", 0);

    // Create renderable.
    auto renderable =
        CustomRenderable::Builder(context()).texcoords(true).normals(true).createSphere(
            desc.radius);
    renderable->setMaterial(material);

    // Create system node.
    auto& new_system_node =
        *parent.getSystemNode().newChild(SystemPosition::origin, Quat::identity);
    new_system_node.data.renderable = renderable;
    return parent.addSatellite(makeUnique<SystemBody>(new_system_node), std::move(orbit));
}
}  // namespace dw