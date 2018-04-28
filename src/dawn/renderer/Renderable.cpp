/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Renderable.h"

namespace dw {
Renderable::Renderable() {
}

Renderable::~Renderable() {
}

void Renderable::setMaterial(SharedPtr<Material> material) {
    material_ = material;
}

Material* Renderable::material() const {
    return material_.get();
}

RenderableNode::RenderableNode() : RenderableNode(nullptr) {
}

RenderableNode::RenderableNode(SharedPtr<Renderable> renderable)
    : renderable_{renderable}, local_offset_{Mat4::identity}, parent_{nullptr} {
}

RenderableNode::RenderableNode(SharedPtr<Renderable> renderable, const Vec3& position,
                               const Quat& orientation, const Vec3& scale)
    : RenderableNode(renderable) {
    setPosition(position);
    setOrientation(orientation);
    setScale(scale);
}

RenderableNode::~RenderableNode() {
}

void RenderableNode::setPosition(const Vec3& position) {
    local_offset_.SetTranslatePart(position);
}

void RenderableNode::setOrientation(const Quat& orientation) {
    local_offset_.Set3x3Part(Mat3{orientation});
}

void RenderableNode::setScale(const Vec3& scale) {
    // TODO.
}

void RenderableNode::addChild(SharedPtr<Renderable> renderable) {
    addChild(makeShared<RenderableNode>(renderable));
}

void RenderableNode::addChild(SharedPtr<RenderableNode> child) {
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

void RenderableNode::drawSceneGraph(Renderer* renderer, uint view, Transform* camera,
                                    const Mat4& model_matrix, const Mat4& view_projection_matrix) {
    Mat4 world_model_matrix = model_matrix * local_offset_;
    if (renderable_) {
        renderable_->draw(renderer, view, camera, world_model_matrix, view_projection_matrix);
    }
    for (auto& c : children_) {
        c->drawSceneGraph(renderer, view, camera, world_model_matrix, view_projection_matrix);
    }
}

Renderable* RenderableNode::renderable() const {
    return renderable_.get();
}
}  // namespace dw
