/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Renderable.h"
#include "renderer/SceneNode.h"

namespace dw {
SceneNode::SceneNode() : SceneNode(nullptr) {
}

SceneNode::SceneNode(SharedPtr<Renderable> renderable)
    : renderable_{renderable}, local_offset_{Mat4::identity}, parent_{nullptr} {
}

SceneNode::SceneNode(SharedPtr<Renderable> renderable, const Vec3& position,
                     const Quat& orientation, const Vec3& scale)
    : SceneNode(renderable) {
    setPosition(position);
    setOrientation(orientation);
    setScale(scale);
}

SceneNode::~SceneNode() {
}

void SceneNode::setPosition(const Vec3& position) {
    local_offset_.SetTranslatePart(position);
}

void SceneNode::setOrientation(const Quat& orientation) {
    local_offset_.Set3x3Part(Mat3{orientation});
}

void SceneNode::setScale(const Vec3& scale) {
    // TODO.
}

void SceneNode::addChild(SharedPtr<Renderable> renderable) {
    addChild(makeShared<SceneNode>(renderable));
}

void SceneNode::addChild(SharedPtr<SceneNode> child) {
    // Detach from existing parent.
    if (child->parent_ != nullptr) {
        auto parent = child->parent_;
        auto it = std::find(parent->children_.begin(), parent->children_.end(), child);
        parent->children_.erase(it);
        child->parent_ = nullptr;
    }

    // Attach child.
    child->parent_ = this;
    children_.push_back(child);
}

void SceneNode::drawSceneGraph(Renderer* renderer, uint view, Transform* camera,
                               const Mat4& model_matrix, const Mat4& view_projection_matrix) {
    Mat4 world_model_matrix = model_matrix * local_offset_;
    if (renderable_) {
        renderable_->draw(renderer, view, camera, world_model_matrix, view_projection_matrix);
    }
    for (auto& c : children_) {
        c->drawSceneGraph(renderer, view, camera, world_model_matrix, view_projection_matrix);
    }
}

Renderable* SceneNode::renderable() const {
    return renderable_.get();
}
}  // namespace dw
