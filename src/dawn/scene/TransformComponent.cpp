/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "scene/TransformComponent.h"

namespace dw {
TransformComponent::TransformComponent(const Position& p, const Quat& o)
    : TransformComponent(p, o, nullptr) {
}

TransformComponent::TransformComponent(const Position& p, const Quat& o, TransformComponent* parent)
    : position(p), orientation(o), relative_to_camera_(false), parent_(nullptr) {
    attachTo(parent);
}

void TransformComponent::setRelativeToCamera(bool relative_to_camera) {
    relative_to_camera_ = relative_to_camera;
}

Mat4 TransformComponent::modelMatrix(const Position& camera_position) const {
    Position origin = relative_to_camera_ ? -camera_position : Position::origin;
    return Mat4::Translate(position.getRelativeTo(origin)).ToFloat4x4() *
           Mat4::FromQuat(orientation);
}

TransformComponent* TransformComponent::parent() const {
    return parent_;
}

void TransformComponent::attachTo(TransformComponent* new_parent) {
    // Detach from existing parent.
    if (parent_ != nullptr) {
        auto it = std::find(parent_->children_.begin(), parent_->children_.end(), this);
        parent_->children_.erase(it);
        parent_ = nullptr;
    }

    // Attach to new parent.
    parent_ = new_parent;
    if (parent_ != nullptr) {
        parent_->children_.push_back(this);
    }
}
}  // namespace dw
