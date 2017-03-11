/*
* Dawn Engine
* Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
*/
#pragma once

#include "ontology/Entity.hpp"

// ReSharper disable CppUnusedIncludeDirective
#include "ecs/Component.h"
// ReSharper restore CppUnusedIncludeDirective

namespace dw {
/// @brief Entity identifier.
using EntityId = Ontology::Entity::ID;

/// @brief Entity object. Currently implemented as an Ontology::Entity wrapper.
class Entity : public Object {
public:
    DW_OBJECT(Entity);

    explicit Entity(Context* context, Ontology::Entity& entity)
        : Object{context}, internal_entity_{entity} {
    }

    virtual ~Entity() = default;

    /// @brief Accesses a component contained within this entity.
    /// @tparam T Component type.
    /// @return The data for this component type.
    template <typename T> T* component() {
        return internal_entity_.getComponentPtr<T>();
    }

    /// @brief Determines whether this entity contains a component type.
    /// @tparam T Component type.
    template <typename T> bool hasComponent() {
        return component<T>() != nullptr;
    }

    /// @brief Initialises and adds a new component to the entity.
    /// @tparam T Component type.
    /// @tparam Args Component constructor argument types.
    /// @param args Component constructor arguments.
    template <typename T, typename... Args> Entity& addComponent(Args... args) {
        return internal_entity_.addComponent<T, Args>(std::forward(args)...);
    };

    /// @brief Returns the identifier of this entity.
    EntityId id() const {
        return internal_entity_.getID();
    }

private:
    Ontology::Entity& internal_entity_;
};
}