/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "scene/Universe.h"
#include "ecs/EntityManager.h"

namespace dw {
EntityManager::EntityManager(Context* context)
    : Object{context},
      entity_manager_{context->ontology_world_.getEntityManager()},
      entity_id_allocator_{1} {
}

Entity& EntityManager::createEntity() {
    return createEntity(entity_id_allocator_++);
}

Entity& EntityManager::createEntity(const Position& p, const Quat& o, Entity* parent) {
    Entity& e = createEntity();
    if (parent) {
        e.addComponent<Transform>(p, o, *parent);
    } else {
        e.addComponent<Transform>(p, o, nullptr);
    }
    return e;
}

Entity& EntityManager::createEntity(const Position& p, const Quat& o) {
    Entity& e = createEntity();
    e.addComponent<Transform>(p, o, subsystem<Universe>()->rootNode());
    return e;
}

Entity* EntityManager::findEntity(EntityId id) {
    auto it = entity_lookup_table_.find(id);
    if (it != entity_lookup_table_.end()) {
        return it->second.get();
    }
    return nullptr;
}

void EntityManager::removeEntity(Entity* entity) {
    entity_lookup_table_.erase(entity->id());
}

Entity& EntityManager::createEntity(EntityId reserved_entity_id) {
    UniquePtr<Entity> entity = makeUnique<Entity>(context(), entity_manager_, reserved_entity_id);
    auto entity_ptr = entity.get();
    entity_lookup_table_.emplace(makePair(entity->id(), std::move(entity)));
    return *entity_ptr;
}
}  // namespace dw
