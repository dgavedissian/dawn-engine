/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "Entity.h"

namespace dw {
Entity::Entity(Context* context, Ontology::EntityManager& entity_manager, EntityId id,
               EntityType type)
    : Object{context},
      id_{id},
      type_{type},
      internal_entity_mgr_{entity_manager},
      internal_entity_id_{entity_manager.createEntity("").getID()},
      transform_{nullptr} {
    entity().addComponent<OntologyMetadata>(this);
}

EntityId Entity::id() const {
    return id_;
}

EntityType Entity::typeId() const {
    return type_;
}

C_Transform* Entity::transform() const {
    return component<C_Transform>();
}
}  // namespace dw
