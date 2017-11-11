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
    return createEntity(reserveEntityId());
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

Entity& EntityManager::createEntity(EntityId reserved_entity_id) {
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

EntityId EntityManager::reserveEntityId() {
    EntityId new_entity_id = entity_id_allocator_++;
    entity_lookup_table_[new_entity_id] = nullptr;
    return new_entity_id;
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
}  // namespace dw
