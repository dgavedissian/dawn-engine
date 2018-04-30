/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Transform.h"

namespace dw {
Transform::Transform(const Position& p, const Quat& o, const Vec3& s)
    : position(p),
      orientation(o),
      scale(s),
      relative_to_camera_(false),
      parent_(nullptr),
      depth_(0) {
}

void Transform::setRelativeToCamera(bool relative_to_camera) {
    relative_to_camera_ = relative_to_camera;
}

Mat4 Transform::modelMatrix(const Position& camera_position) const {
    Position origin = relative_to_camera_ ? -camera_position : Position::origin;
    return Mat4::Scale(scale) * Mat4::Translate(position.getRelativeTo(origin)).ToFloat4x4() *
           Mat4::FromQuat(orientation);
}

Transform* Transform::parent() const {
    return parent_;
}

void Transform::addChild(Transform* child) {
    child->detachFromParent();
    child->parent_ = this;
    child->depth_ = depth_ + static_cast<byte>(1);
    children_.emplace_back(child);
}

void Transform::addChild(LocalTransform* child) {
    child->detachFromParent();
    child->t_parent_ = this;
    child->depth_ = 0;
    lt_children_.emplace_back(child);
}

void Transform::detachFromParent() {
    if (parent_) {
        auto it = std::find(parent_->children_.begin(), parent_->children_.end(), this);
        parent_->children_.erase(it);
        parent_ = nullptr;
        depth_ = 0;
    }
}

LocalTransform::LocalTransform(const Vec3& p, const Quat& o, const Vec3& s)
    : position(p), orientation(o), scale(s), lt_parent_(nullptr), t_parent_(nullptr), depth_(0) {
}

void LocalTransform::addChild(LocalTransform* child) {
    child->detachFromParent();
    child->lt_parent_ = this;
    child->depth_ = depth_ + static_cast<byte>(1);
    children_.emplace_back(child);
}

void LocalTransform::detachFromParent() {
    if (lt_parent_) {
        auto it = std::find(lt_parent_->children_.begin(), lt_parent_->children_.end(), this);
        lt_parent_->children_.erase(it);
        lt_parent_ = nullptr;
        depth_ = 0;
    } else if (t_parent_) {
        auto it = std::find(t_parent_->lt_children_.begin(), t_parent_->lt_children_.end(), this);
        t_parent_->lt_children_.erase(it);
        t_parent_ = nullptr;
        depth_ = 0;
    }
}
}  // namespace dw
