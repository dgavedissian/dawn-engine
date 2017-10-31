/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "ecs/EntityManager.h"
#include "renderer/Renderable.h"
#include "scene/Universe.h"
#include "renderer/MeshBuilder.h"
#include "resource/ResourceCache.h"

namespace dw {
Universe::Universe(Context* context) : Object(context) {
    root_node_ = makeShared<Transform>(Vec3::zero, Quat::identity, nullptr);

    background_renderable_root_ = makeShared<RenderableNode>();
    background_entity_ = &subsystem<EntityManager>()
                              ->createEntity(Vec3::zero, Quat::identity, nullptr)
                              .addComponent<RenderableComponent>(background_renderable_root_);
    background_entity_->transform()->setRelativeToCamera(true);
}

Universe::~Universe() {
}

void Universe::createStarSystem() {
    auto vs = subsystem<ResourceCache>()->get<VertexShader>("/base/space/skybox.vs");
    auto fs = subsystem<ResourceCache>()->get<FragmentShader>("/base/space/skybox_starfield.fs");
    auto background_material =
        makeShared<Material>(context(), makeShared<Program>(context(), vs, fs));
    background_material->setTextureUnit(
        subsystem<ResourceCache>()->get<Texture>("/base/space/starfield.jpg"));
    background_material->setUniform<int>("starfield_sampler", 0);
    auto skybox = MeshBuilder{context()}.normals(false).texcoords(true).createBox(-10000.0f);
    skybox->setMaterial(background_material);
    background_renderable_root_->addChild(skybox);
}

void Universe::update(float) {
}

Transform* Universe::rootNode() const {
    return root_node_.get();
}
}  // namespace dw
