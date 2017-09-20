/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "ecs/Entity.h"
#include "scene/Transform.h"

namespace dw {
    Transform::Transform(const Position& p, const Quat& o, Entity& entity) : Transform{p, o, entity.component<Transform>()} {}
    Transform::Transform(const Position &p, const Quat &o, Transform* parent) : position_{ p }, orientation_{ o }, parent_{ nullptr } {
    attachTo(parent);
}

Position &Transform::position() {
    return position_;
}

const Position &Transform::position() const {
    return position_;
}

Quat& Transform::orientation() {
    return orientation_;
}

const Quat& Transform::orientation() const {
    return orientation_;
}

Transform * Transform::parent()
{
    return parent_;
}

void Transform::attachTo(Transform* new_parent)
{
    // Detach from existing parent.
    if (parent_ != nullptr)
    {
        auto it = std::find(parent_->children_.begin(), parent_->children_.end(), this);
        parent_->children_.erase(it);
        parent_ = nullptr;
    }

    // Attach to new parent.
    parent_ = new_parent;
    if (parent_ != nullptr)
    {
        parent_->children_.push_back(this);
    }
}
}  // namespace dw
