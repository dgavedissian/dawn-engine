/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "ecs/Entity.h"

namespace dw {
Entity::Entity(Context* context, Ontology::EntityManager& entity_manager,
               Ontology::Entity::ID entity_id)
    : Object{context},
      entity_manager_{entity_manager},
      internal_entity_id_{entity_id},
      transform_{nullptr} {
}

EntityId Entity::id() const {
    return internal_entity_id_;
}

Transform* Entity::transform() const {
    return component<Transform>();
}
}  // namespace dw
