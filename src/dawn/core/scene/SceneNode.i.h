#include "SceneNode.h"
/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

namespace dw {
namespace detail {
template <typename T>
LargeSceneNode<T>* SceneNodePool<T>::newLargeSceneNode(const LargePosition& p, const Quat& o) {
    return new LargeSceneNode<T>(this, p, o);
}

template <typename T>
SceneNode<T>* SceneNodePool<T>::newSceneNode(const Vec3& p, const Quat& o, const Vec3& s) {
    return new SceneNode<T>(this, p, o, s);
}

template <typename T> void SceneNodePool<T>::free(LargeSceneNode<T>* node) {
    delete node;
}

template <typename T> void SceneNodePool<T>::free(SceneNode<T>* node) {
    delete node;
}
}  // namespace detail

template <typename T>
LargeSceneNode<T>::LargeSceneNode(detail::SceneNodePool<T>* pool, const LargePosition& p,
                                  const Quat& o)
    : position(p),
      orientation(o),
      relative_to_camera_(false),
      parent_(nullptr),
      depth_(0),
      pool_(pool) {
}

template <typename T> void LargeSceneNode<T>::setRelativeToCamera(bool relative_to_camera) {
    relative_to_camera_ = relative_to_camera;
}

template <typename T>
Mat4 LargeSceneNode<T>::calculateModelMatrix(const LargePosition& camera_position) const {
    return Mat4::Translate(position.getRelativeTo(camera_position)).ToFloat4x4() *
           Mat4::FromQuat(orientation);
}

template <typename T> Mat4 LargeSceneNode<T>::calculateViewMatrix() const {
    return Mat4::FromQuat(orientation).Inverted();
}

template <typename T> LargeSceneNode<T>* LargeSceneNode<T>::parent() const {
    return parent_;
}

template <typename T> LargeSceneNode<T>* LargeSceneNode<T>::addChild(LargeSceneNode<T>* child) {
    child->detachFromParent();
    child->parent_ = this;
    child->depth_ = depth_ + static_cast<byte>(1);
    large_children_.emplace_back(child);
    return child;
}

template <typename T> SceneNode<T>* LargeSceneNode<T>::addChild(SceneNode<T>* child) {
    child->detachFromParent();
    child->large_parent_ = this;
    child->depth_ = 0;
    children_.emplace_back(child);
    return child;
}

template <typename T>
LargeSceneNode<T>* LargeSceneNode<T>::newLargeChild(const LargePosition& p, const Quat& o) {
    auto* node = pool_->newLargeSceneNode(p, o);
    return addChild(node);
}

template <typename T>
SceneNode<T>* LargeSceneNode<T>::newChild(const Vec3& p, const Quat& o, const Vec3& s) {
    auto* node = pool_->newSceneNode(p, o, s);
    return addChild(node);
}

template <typename T> LargeSceneNode<T>* LargeSceneNode<T>::largeChild(int i) {
    return large_children_[i];
}

template <typename T> SceneNode<T>* LargeSceneNode<T>::child(int i) {
    return children_[i];
}

template <typename T> int LargeSceneNode<T>::largeChildCount() {
    return static_cast<int>(large_children_.size());
}

template <typename T> int LargeSceneNode<T>::childCount() {
    return static_cast<int>(children_.size());
}

template <typename T> void LargeSceneNode<T>::detachFromParent() {
    if (parent_) {
        auto it = std::find(parent_->large_children_.begin(), parent_->large_children_.end(), this);
        parent_->large_children_.erase(it);
        parent_ = nullptr;
        depth_ = 0;
    }
}

template <typename T>
SceneNode<T>::SceneNode(detail::SceneNodePool<T>* pool, const Vec3& p, const Quat& o, const Vec3& s)
    : dirty(true),
      transform_{p, o, s},
      parent_(nullptr),
      large_parent_(nullptr),
      depth_(0),
      pool_(pool) {
}

template <typename T> Mat4 SceneNode<T>::calculateModelMatrix() const {
    return transform_.calculateModelMatrix();
}

template <typename T> LargeSceneNode<T>* SceneNode<T>::largeParent() const {
    return large_parent_;
}

template <typename T> SceneNode<T>* SceneNode<T>::parent() const {
    return parent_;
}

template <typename T> SceneNode<T>* SceneNode<T>::addChild(SceneNode<T>* child) {
    child->detachFromParent();
    child->parent_ = this;
    child->depth_ = depth_ + static_cast<byte>(1);
    children_.emplace_back(child);
    return child;
}

template <typename T>
SceneNode<T>* SceneNode<T>::newChild(const Vec3& p, const Quat& o, const Vec3& s) {
    auto* node = pool_->newSceneNode(p, o, s);
    return addChild(node);
}

template <typename T> SceneNode<T>* SceneNode<T>::child(int i) {
    return children_[i];
}

template <typename T> int SceneNode<T>::childCount() {
    return static_cast<int>(children_.size());
}

template <typename T> detail::Transform& SceneNode<T>::transform() {
    dirty = true;
    return transform_;
}

template <typename T> const detail::Transform& SceneNode<T>::transform() const {
    return transform_;
}

template <typename T> void SceneNode<T>::detachFromParent() {
    if (parent_) {
        auto it = std::find(parent_->children_.begin(), parent_->children_.end(), this);
        parent_->children_.erase(it);
        parent_ = nullptr;
        depth_ = 0;
    } else if (large_parent_) {
        auto it = std::find(large_parent_->children_.begin(), large_parent_->children_.end(), this);
        large_parent_->children_.erase(it);
        large_parent_ = nullptr;
        depth_ = 0;
    }
}
}  // namespace dw
