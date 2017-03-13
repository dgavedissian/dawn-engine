/*
* Dawn Engine
* Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
*/
#pragma once

#include "ecs/Entity.h"

namespace dw {
class EntityManager : public Object {
public:
    DW_OBJECT(EntityManager);

    EntityManager(Context* context);
    virtual ~EntityManager() = default;

    /// Creates a new empty entity.
    /// @return A newly created entity.
    Entity* createEntity();

    /// Looks up an entity by its ID.
    /// @param id Entity ID.
    /// @return The entity which corresponds to this entity ID.
    Entity* getEntity(EntityId id);

    /// Removes an entity from the engine.
    /// @param entity Entity to remove.
    void removeEntity(Entity* entity);

private:
    Ontology::EntityManager& entity_manager_;
    HashMap<EntityId, UniquePtr<Entity>> entity_lookup_table_;
};
}