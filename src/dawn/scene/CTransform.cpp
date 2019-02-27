/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "scene/CTransform.h"

namespace dw {
CTransform::CTransform(Node* scene_node) : node(scene_node) {
}

CTransform::CTransform(Node* scene_node, SharedPtr<Renderable> renderable) : node(scene_node) {
    setRenderable(renderable);
}

void CTransform::attachTo(CTransform* new_parent) {
    new_parent->node->addChild(node);
}

void CTransform::setRenderable(SharedPtr<Renderable> renderable) {
    node->data.renderable = renderable;
}

SharedPtr<Renderable> CTransform::renderable() const {
    return node->data.renderable;
}
}  // namespace dw
