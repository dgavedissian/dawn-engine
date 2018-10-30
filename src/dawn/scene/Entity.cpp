/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "Entity.h"

namespace dw {
Entity::Entity(Context* ctx, SceneManager* scene_manager, Ontology::EntityManager& entity_manager,
               EntityId id, EntityType type)
    : Object{ctx},
      scene_manager_{scene_manager},
      id_{id},
      type_{type},
      internal_entity_mgr_{entity_manager},
      internal_entity_id_{entity_manager.createEntity("").getID()} {
    entity().addComponent<OntologyMetadata>(this);
}

SceneManager* Entity::sceneManager() const {
    return scene_manager_;
}

EntityId Entity::id() const {
    return id_;
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
