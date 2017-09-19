/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "ecs/Entity.h"

namespace dw {
Entity::Entity(Context* context, EntityId id)
        : Object{context}, internal_id_(id) {
}

void Entity::update(float dt) {
    for (auto& c : component_data_) {
        c.second->update(dt);
    }
}

EntityId Entity::id() const {
    return internal_id_;
}

Transform* Entity::transform() {
    return transform_;
}
}