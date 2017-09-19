/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "ecs/Component.h"
#include "scene/Transform.h"

namespace dw {
/// Entity identifier.
using EntityId = u32;

/// Entity object.
class Entity : public Object {
public:
    DW_OBJECT(Entity);

    // TODO: owning world.
    Entity(Context* context, EntityId id);
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

    /// Updates this entity.
    /// @param dt Delta time.
    void update(float dt);

    /// Returns the identifier of this entity.
    EntityId id() const;

    /// Get the current transform.
    Transform* transform();

private:
    HashMap<TypeId, UniquePtr<Component>> component_data_;

    EntityId internal_id_;
    Transform* transform_;
};

template <typename T> T* Entity::component() const {
    auto data = component_data_.find(T::typeStatic());
    if (data == component_data_.end()) {
        return nullptr;
    }
    return data->second.get();
}

template <> Transform* Entity::component<Transform>() const {
    return transform_;
}

template <typename T> bool Entity::hasComponent() const {
    return component<T>() != nullptr;
}

template <typename T, typename... Args> Entity& Entity::addComponent(Args... args) {
    component_data_.emplace({T::typeStatic(), makeUnique<Args>(std::forward<Args>(args)...)});
    return *this;
}

template <typename... Args> Entity& Entity::addComponent<Transform>(Args... args) {
    addComponent<Transform, Args>(std::forward<Args>(args)...);
    transform_ = component<Transform>();
    return *this;
}
}  // namespace dw
