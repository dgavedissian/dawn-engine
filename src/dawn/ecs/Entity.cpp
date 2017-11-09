/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "ecs/Entity.h"

namespace dw {
Entity::Entity(Context* context, Ontology::EntityManager& entity_manager,          EntityId id)
    : Object{context},
      id_{id},
      entity_manager_{entity_manager},
      internal_entity_id_{0},
      transform_{nullptr} {
    internal_entity_id_ = entity_manager_.createEntity("").getID();
}

EntityId Entity::id() const {
    return id_;
}

Transform* Entity::transform() const {
    return component<Transform>();
}
}  // namespace dw
