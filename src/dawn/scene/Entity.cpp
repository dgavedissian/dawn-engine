/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "Entity.h"
#include "SceneManager.h"

namespace dw {
Entity::Entity(SceneManager* sceneManager, EntityId id, EntityType type)
    : registry_{sceneManager->registry_},
      entity_{id},
      type_{type} {
}

EntityId Entity::id() const {
    return entity_;
}

EntityType Entity::typeId() const {
    return type_;
}

detail::Transform* Entity::transform() {
    auto transform_component = component<CTransform>();
    if (transform_component) {
        return &transform_component->node->transform();
    }
    return nullptr;
}

const detail::Transform* Entity::transform() const {
    const auto transform_component = component<CTransform>();
    if (transform_component) {
        const auto* node = transform_component->node;
        return &node->transform();
    }
    return nullptr;
}
}  // namespace dw
