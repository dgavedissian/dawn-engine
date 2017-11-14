/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "ecs/Entity.h"

namespace dw {
class EntityManager : public Subsystem {
public:
    DW_OBJECT(EntityManager);

    EntityManager(Context* context);
    virtual ~EntityManager() = default;

    /// Creates a new empty entity.
    /// @return A newly created entity.
    Entity& createEntity();

    /// Creates a new entity with a transform component.
    /// @param p Initial position.
    /// @param o Initial orientation.
    /// @param parent Parent entity.
    /// @return A newly created entity.
    Entity& createEntity(const Position& p, const Quat& o, Entity* parent);

    /// Creates a new entity with a transform component.
    /// @param p Initial position.
    /// @param o Initial orientation.
    /// @return A newly created entity.
    Entity& createEntity(const Position& p, const Quat& o);

    /// Creates a new empty entity with a previously reserved entity ID.
    /// @return A newly created entity.
    Entity& createEntity(EntityId reserved_entity_id);

    /// Reserve a new entity ID.
    /// @return Unique unused entity ID.
    EntityId reserveEntityId();

    /// Looks up an entity by its ID.
    /// @param id Entity ID.
    /// @return The entity which corresponds to this entity ID.
    Entity* findEntity(EntityId id);

    /// Removes an entity from the engine.
    /// @param entity Entity to remove.
    void removeEntity(Entity* entity);

private:
    Ontology::EntityManager& entity_manager_;
    HashMap<EntityId, UniquePtr<Entity>> entity_lookup_table_;
    EntityId entity_id_allocator_;

    friend class NetSystem;
};
}  // namespace dw