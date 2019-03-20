/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "scene/CSceneNode.h"
#include "CSceneNode.h"


namespace dw {
CSceneNode::CSceneNode(Node* scene_node) : node(scene_node) {
}

CSceneNode::CSceneNode(Node* scene_node, SharedPtr<Renderable> renderable) : node(scene_node) {
    setRenderable(renderable);
}

void CSceneNode::attachTo(CSceneNode* new_parent) {
    new_parent->node->addChild(node);
}

detail::Transform &CSceneNode::transform() {
    return node->transform();
}

const detail::Transform &CSceneNode::transform() const {
    return node->transform();
}

void CSceneNode::setRenderable(SharedPtr<Renderable> renderable) {
    node->data.renderable = std::move(renderable);
}

SharedPtr<Renderable> CSceneNode::renderable() const {
    return node->data.renderable;
}
}  // namespace dw
