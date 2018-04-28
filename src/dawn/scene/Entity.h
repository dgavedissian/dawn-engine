/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "ontology/Entity.hpp"

#include "Component.h"
#include "scene/Transform.h"

namespace dw {
/// Entity identifier.
using EntityId = u64;
using EntityType = u32;

/// Entity object. Currently implemented as an Ontology::Entity wrapper.
class Entity : public Object {
public:
    DW_OBJECT(Entity);

    explicit Entity(Context* context, Ontology::EntityManager& entity_manager, EntityId id,
                    EntityType type);
    virtual ~Entity() = default;

    /// Accesses a component contained within this entity.
    /// @tparam T Component type.
    /// @return The data for this component type.
    template <typename T> T* component() const;

    /// Determines whether this entity contains a component type.
    /// @tparam T Component type.
    template <typename T> bool hasComponent() const;

    /// Initialises and adds a new component to the entity.
    /// @tparam T Component type.
    /// @tparam Args Component constructor argument types.
    /// @param args Component constructor arguments.
    template <typename T, typename... Args> Entity& addComponent(Args... args);

    /// Returns the identifier of this entity.
    EntityId id() const;

    /// Returns the type ID of this entity.
    EntityType typeId() const;

    /// Shortcut for returning the transform component.
    Transform* transform() const;

private:
    Ontology::EntityManager& internal_entity_mgr_;
    Ontology::Entity::ID internal_entity_id_;
    EntityId id_;
    EntityType type_;

    Ontology::Entity& entity() const {
        return internal_entity_mgr_.getEntity(internal_entity_id_);
    }

    Transform* transform_;
};

template <typename T> T* Entity::component() const {
    return entity().hasComponent<T>() ? entity().getComponentPtr<T>() : nullptr;
}

template <typename T> bool Entity::hasComponent() const {
    return component<T>() != nullptr;
}

template <typename T, typename... Args> Entity& Entity::addComponent(Args... args) {
    entity().addComponent<T, Args...>(std::forward<Args>(args)...);
    entity().getComponentPtr<T>()->onAddToEntity(this);
    return *this;
}

/// Ontology metadata component.
struct OntologyMetadata : public Component {
    Entity* entity_ptr;
    explicit OntologyMetadata(Entity* entity_ptr) : entity_ptr(entity_ptr) {
    }
};
}  // namespace dw
