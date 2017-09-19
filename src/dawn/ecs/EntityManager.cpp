/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "ecs/EntityManager.h"

namespace dw {
EntityManager::EntityManager(Context* context)
    : Object{context}, id_allocator(1) {
}

Entity& EntityManager::createEntity() {
    // TODO: Real ID allocator.
    UniquePtr<Entity> entity = makeUnique<Entity>(context(), id_allocator++);
    auto entity_ptr = entity.get();
    entity_lookup_table_.emplace(makePair(entity->id(), std::move(entity)));
    return *entity_ptr;
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

void EntityManager::update(float dt) {
    for (auto& entity_pair : entity_lookup_table_) {
        entity_pair.second->update(dt);
    }
}
}  // namespace dw
