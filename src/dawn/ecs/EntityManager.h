/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "ecs/Entity.h"

namespace dw {
class DW_API EntityManager : public Object {
public:
    DW_OBJECT(EntityManager);

    EntityManager(Context* context);
    ~EntityManager() = default;

    /// Creates a new empty entity.
    /// @return A newly created entity.
    Entity& createEntity();

    /// Looks up an entity by its ID.
    /// @param id Entity ID.
    /// @return The entity which corresponds to this entity ID.
    Entity* findEntity(EntityId id);

    /// Removes an entity from the engine.
    /// @param entity Entity to remove.
    void removeEntity(Entity* entity);

    /// Update all entities.
    /// @param dt Delta time.
    void update(float dt);

private:
    HashMap<EntityId, UniquePtr<Entity>> entity_lookup_table_;
    EntityId id_allocator;
};
}  // namespace dw