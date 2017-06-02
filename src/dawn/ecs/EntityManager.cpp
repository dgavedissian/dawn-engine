/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "ecs/EntityManager.h"

namespace dw {
EntityManager::EntityManager(Context* context)
    : Object{context}, entity_manager_{context->ontology_world_.getEntityManager()} {
}

Entity* EntityManager::createEntity() {
    UniquePtr<Entity> entity = makeUnique<Entity>(context(), entity_manager_.createEntity(""));
    Entity* entityPtr = entity.get();
    entity_lookup_table_.emplace(makePair(entity->id(), std::move(entity)));
    return entityPtr;
}

Entity* EntityManager::getEntity(EntityId id) {
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
