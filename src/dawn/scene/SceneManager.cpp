/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "renderer/Renderable.h"
#include "scene/SceneManager.h"
#include "renderer/MeshBuilder.h"
#include "resource/ResourceCache.h"
#include "renderer/Renderer.h"
#include "scene/PhysicsScene.h"
#include "SLinearMotion.h"
#include "net/CNetTransform.h"
#include "renderer/SceneGraph.h"
#include "SceneManager.h"

namespace dw {
SceneManager::SceneManager(Context* ctx, EventSystem* event_system, SceneGraph* scene_graph)
    : Object(ctx), background_scene_node_(nullptr) {
    background_scene_node_ = scene_graph->backgroundNode().newChild();

    physics_scene_ = makeUnique<PhysicsScene>(ctx, this, event_system);

    // Set up built in entity systems.
    scene_graph->setupEntitySystems(this);
    addSystem<SLinearMotion>();
    addSystem<SNetTransformSync>();
}

SceneManager::~SceneManager() {
    physics_scene_.reset();
}

void SceneManager::createStarSystem() {
    auto vs = *module<ResourceCache>()->get<VertexShader>("base:space/skybox.vs");
    auto fs = *module<ResourceCache>()->get<FragmentShader>("base:space/skybox_starfield.fs");
    auto background_material =
        makeShared<Material>(context(), makeShared<Program>(context(), vs, fs));
    background_material->setTexture(
        *module<ResourceCache>()->get<Texture>("base:space/starfield.jpg"));
    background_material->setUniform<int>("starfield_sampler", 0);
    background_material->setDepthWrite(false);
    auto skybox = MeshBuilder{context()}.normals(false).texcoords(true).createBox(-100.0f);
    skybox->setMaterial(background_material);
    background_scene_node_->data.renderable = skybox;
}

Entity& SceneManager::createEntity(EntityType type) {
    auto new_entity = registry_.create();

    // Look up slot and move new entity into it.
    entity_lookup_table_[new_entity] = makeUnique<Entity>(this, new_entity, type);
    return *entity_lookup_table_[new_entity];
}

Entity& SceneManager::createEntity(EntityType type, const Vec3& p, const Quat& o, Frame& frame,
                                   SharedPtr<Renderable> renderable) {
    Entity& e = createEntity(type);
    e.addComponent<CSceneNode>(frame.newChild(p, o), renderable);
    return e;
}

Entity& SceneManager::createEntity(EntityType type, const Vec3& p, const Quat& o, Entity& parent,
                                   SharedPtr<Renderable> renderable) {
    Entity& e = createEntity(type);
    e.addComponent<CSceneNode>(parent.component<CSceneNode>()->node->newChild(p, o), renderable);
    return e;
}

Entity* SceneManager::findEntity(EntityId id) {
    auto it = entity_lookup_table_.find(id);
    if (it != entity_lookup_table_.end()) {
        return it->second.get();
    }
    return nullptr;
}

void SceneManager::removeEntity(Entity* entity) {
    auto id = entity->id();
    // Erase from the lookup table (and delete the memory).
    entity_lookup_table_.erase(id);
    registry_.destroy(id);
}

void SceneManager::update(float dt) {
    for (auto& s : system_process_order_) {
        s->process(this, dt);
    }
    physics_scene_->update(dt, nullptr);
}

PhysicsScene* SceneManager::physicsScene() const {
    return physics_scene_.get();
}
}  // namespace dw
