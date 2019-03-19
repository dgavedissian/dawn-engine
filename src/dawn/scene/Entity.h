/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#pragma once

#include <entt/entt.hpp>

#include "Component.h"
#include "scene/CTransform.h"

namespace dw {
class SceneManager;

/// Entity identifier.
using EntityId = u64;
using EntityType = u32;

/// Entity object.
class Entity {
public:
    explicit Entity(SceneManager* sceneManager, EntityId id, EntityType type = 0);
    virtual ~Entity() = default;

    /// Accesses a component contained within this entity.
    /// @tparam T Component type.
    /// @return The data for this component type.
    template <typename T> T* component() const;

    /// Determines whether this entity contains one or more component types.
    /// @tparam T Component types.
    template <typename... T> bool hasComponent() const;

    /// Initialises and adds a new component to the entity.
    /// @tparam T Component type.
    /// @tparam Args Component constructor argument types.
    /// @param args Component constructor arguments.
    template <typename T, typename... Args> Entity& addComponent(Args... args);

    /// Removes a component of a specific type from the entity.
    /// @tparam T Component type.
    template <typename T> void removeComponent();

    /// Returns the identifier of this entity.
    EntityId id() const;

    /// Returns the type ID of this entity.
    EntityType typeId() const;

    /// Shortcut for returning the transform stored inside the scene node of the transform
    /// component.
    detail::Transform* transform();

    /// Shortcut for returning the transform stored inside the scene node of the transform
    /// component.
    const detail::Transform* transform() const;

private:
    entt::basic_registry<EntityId>& registry_;
    EntityId entity_;
    EntityType type_;
};

template <typename T> T* Entity::component() const {
    return registry_.try_get<T>(entity_);
}

template <typename... T> bool Entity::hasComponent() const {
    return registry_.has<T...>(entity_);
}

template <typename T, typename... Args> Entity& Entity::addComponent(Args... args) {
    registry_.assign_or_replace<T>(entity_, args...);
    registry_.get<T>(entity_).onAddToEntity(this);
    return *this;
}

template<typename T>
void Entity::removeComponent() {
    registry_.remove<T>(entity_);
}
}  // namespace dw
