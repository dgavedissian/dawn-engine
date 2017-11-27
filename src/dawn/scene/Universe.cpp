/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Renderable.h"
#include "scene/Universe.h"
#include "renderer/MeshBuilder.h"
#include "resource/ResourceCache.h"

namespace dw {
Universe::Universe(Context* ctx)
    : Subsystem(ctx),
      entity_manager_(ctx->ontology_world_.getEntityManager()),
      entity_id_allocator_(1) {
    setDependencies<ResourceCache>();

    root_node_ = makeShared<Transform>(Position::origin, Quat::identity, nullptr);

    background_renderable_root_ = makeShared<RenderableNode>();
    background_entity_ = &createEntity(Position::origin, Quat::identity, nullptr)
                              .addComponent<RenderableComponent>(background_renderable_root_);
    background_entity_->transform()->setRelativeToCamera(true);
}

Universe::~Universe() {
}

void Universe::createStarSystem() {
    auto vs = subsystem<ResourceCache>()->get<VertexShader>("base:space/skybox.vs");
    auto fs = subsystem<ResourceCache>()->get<FragmentShader>("base:space/skybox_starfield.fs");
    auto background_material =
        makeShared<Material>(context(), makeShared<Program>(context(), vs, fs));
    background_material->setTexture(
        subsystem<ResourceCache>()->get<Texture>("base:space/starfield.jpg"));
    background_material->setUniform<int>("starfield_sampler", 0);
    auto skybox = MeshBuilder{context()}.normals(false).texcoords(true).createBox(-10000.0f);
    skybox->setMaterial(background_material);
    background_renderable_root_->addChild(skybox);
}

Entity& Universe::createEntity() {
    return createEntity(reserveEntityId());
}

Entity& Universe::createEntity(const Position& p, const Quat& o, Entity* parent) {
    Entity& e = createEntity();
    if (parent) {
        e.addComponent<Transform>(p, o, *parent);
    } else {
        e.addComponent<Transform>(p, o, nullptr);
    }
    return e;
}

Entity& Universe::createEntity(const Position& p, const Quat& o) {
    Entity& e = createEntity();
    e.addComponent<Transform>(p, o, rootNode());
    return e;
}

Entity& Universe::createEntity(EntityId reserved_entity_id) {
    // Add to entity lookup table if reserved from elsewhere (i.e. server).
    if (entity_lookup_table_.find(reserved_entity_id) == entity_lookup_table_.end()) {
        entity_lookup_table_[reserved_entity_id] = nullptr;
    }

    // Look up slot and move new entity into it.
    auto entity_slot = entity_lookup_table_.find(reserved_entity_id);
    assert(entity_slot != entity_lookup_table_.end() && entity_slot->second == nullptr);
    UniquePtr<Entity> entity = makeUnique<Entity>(context(), entity_manager_, reserved_entity_id);
    auto entity_ptr = entity.get();
    entity_slot->second = std::move(entity);
    return *entity_ptr;
}

EntityId Universe::reserveEntityId() {
    EntityId new_entity_id = entity_id_allocator_++;
    entity_lookup_table_[new_entity_id] = nullptr;
    return new_entity_id;
}

Entity* Universe::findEntity(EntityId id) {
    auto it = entity_lookup_table_.find(id);
    if (it != entity_lookup_table_.end()) {
        return it->second.get();
    }
    return nullptr;
}

void Universe::removeEntity(Entity* entity) {
    entity_lookup_table_.erase(entity->id());
}

void Universe::update(float) {
}

Transform* Universe::rootNode() const {
    return root_node_.get();
}
}  // namespace dw
