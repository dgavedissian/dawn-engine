/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Node.h"

namespace dw {
namespace detail {
Transform::Transform(Vec3 p, Quat o, Vec3 s) : position(p), orientation(o), scale(s) {
    assert(abs(s.x) > 0.0001f && abs(s.y) > 0.0001f && abs(s.z) > 0.0001f);
}

Mat4 Transform::toMat4() const {
    Mat4 scale_matrix = Mat4::Scale(scale).ToFloat4x4();
    Mat4 rotate_matrix = Mat4::FromQuat(orientation);
    Mat4 translate_matrix = Mat4::Translate(position).ToFloat4x4();
    return translate_matrix * rotate_matrix * scale_matrix;
}

Transform Transform::fromMat4(const Mat4& matrix) {
    return Transform{matrix.TranslatePart(), matrix.RotatePart().ToQuat(), matrix.ExtractScale()};
}

SystemNode* SceneNodePool::newSystemNode(const SystemPosition& p, const Quat& o) {
    return new SystemNode(this, p, o);
}

Node* SceneNodePool::newNode(Frame* frame, const Vec3& p, const Quat& o, const Vec3& s) {
    return new Node(this, frame, p, o, s);
}

void SceneNodePool::free(SystemNode* node) {
    delete node;
}

void SceneNodePool::free(Node* node) {
    delete node;
}
}  // namespace detail

SystemNode::SystemNode(detail::SceneNodePool* pool, const SystemPosition& p, const Quat& o)
    : position(p), orientation(o), parent_(nullptr), depth_(0), pool_(pool) {
}

Mat4 SystemNode::calculateModelMatrix(const SystemPosition& camera_position) const {
    return Mat4::Translate(position.getRelativeTo(camera_position)).ToFloat4x4() *
           Mat4::FromQuat(orientation);
}

Mat4 SystemNode::calculateViewMatrix() const {
    return Mat4::FromQuat(orientation).Inverted();
}

SystemNode* SystemNode::parent() const {
    return parent_;
}

SystemNode* SystemNode::addChild(SystemNode* child) {
    child->detachFromParent();
    child->parent_ = this;
    child->depth_ = depth_ + static_cast<byte>(1);
    children_.emplace_back(child);
    return child;
}

SystemNode* SystemNode::newChild(const SystemPosition& p, const Quat& o) {
    auto* node = pool_->newSystemNode(p, o);
    return addChild(node);
}

SystemNode* SystemNode::child(int i) {
    return children_[i];
}

int SystemNode::childCount() const {
    return static_cast<int>(children_.size());
}

void SystemNode::detachFromParent() {
    if (parent_) {
        auto it = std::find(parent_->children_.begin(), parent_->children_.end(), this);
        parent_->children_.erase(it);
        parent_ = nullptr;
        depth_ = 0;
    }
}

Node::Node(detail::SceneNodePool* pool, Frame* frame, const Vec3& p, const Quat& o, const Vec3& s)
    : dirty(true), transform_{p, o, s}, parent_(nullptr), frame_(frame), depth_(0), pool_(pool) {
}

Mat4 Node::calculateModelMatrix() const {
    return transform_.toMat4();
}

Frame* Node::frame() const {
    return frame_;
}

Node* Node::parent() const {
    return parent_;
}

Node* Node::addChild(Node* child) {
    child->detachFromParent();
    child->parent_ = this;
    child->depth_ = depth_ + static_cast<byte>(1);
    children_.emplace_back(child);
    return child;
}

Node* Node::newChild(const Vec3& p, const Quat& o, const Vec3& s) {
    auto* node = pool_->newNode(frame_, p, o, s);
    return addChild(node);
}

Node* Node::child(int i) {
    return children_[i];
}

int Node::childCount() const {
    return static_cast<int>(children_.size());
}

detail::Transform& Node::transform() {
    dirty = true;
    return transform_;
}

const detail::Transform& Node::transform() const {
    return transform_;
}

void Node::detachFromParent() {
    if (parent_) {
        auto it = std::find(parent_->children_.begin(), parent_->children_.end(), this);
        parent_->children_.erase(it);
        parent_ = nullptr;
        depth_ = 0;
    }
}

Frame::Frame(SystemNode* system_node)
    : pool_(system_node->pool_),
      system_node_(system_node),
      root_frame_node_(nullptr, {system_node->pool_}),
      followed_(nullptr) {
    root_frame_node_.reset(pool_->newNode(this, Vec3::zero, Quat::identity, Vec3::one));
}

Node* Frame::addChild(Node* child) {
    return root_frame_node_->addChild(child);
}

Node* Frame::newChild(const Vec3& p, const Quat& o, const Vec3& s) {
    return root_frame_node_->newChild(p, o, s);
}

Node* Frame::child(int i) {
    return root_frame_node_->child(i);
}

int Frame::childCount() const {
    return root_frame_node_->childCount();
}

void Frame::setFollowed(Node* followed) {
    followed_ = followed;
}

const SystemPosition& Frame::position() const {
    return system_node_->position;
}
}  // namespace dw
