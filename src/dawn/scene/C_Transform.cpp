/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "scene/C_Transform.h"

namespace dw {
C_Transform::C_Transform(LargeSceneNodeR* scene_node) : node(scene_node) {
}

C_Transform::C_Transform(LargeSceneNodeR* scene_node, SharedPtr<Renderable> renderable)
    : node(scene_node) {
    setRenderable(renderable);
}

C_Transform::C_Transform(SceneNodeR* scene_node) : node(scene_node) {
}

C_Transform::C_Transform(SceneNodeR* scene_node, SharedPtr<Renderable> renderable)
    : node(scene_node) {
    setRenderable(renderable);
}

void C_Transform::attachTo(C_Transform* new_parent) {
    node.match(
        [new_parent](LargeSceneNodeR* scene_node) {
            new_parent->node.get<LargeSceneNodeR*>()->addChild(scene_node);
        },
        [new_parent](SceneNodeR* scene_node) {
            new_parent->node.match(
                [scene_node](LargeSceneNodeR* parent_scene_node) {
                    parent_scene_node->addChild(scene_node);
                },
                [scene_node](SceneNodeR* parent_scene_node) {
                    parent_scene_node->addChild(scene_node);
                });
        });
}

void C_Transform::setRenderable(SharedPtr<Renderable> renderable) {
    node.match(
        [renderable](LargeSceneNodeR* scene_node) { scene_node->data.renderable = renderable; },
        [renderable](SceneNodeR* scene_node) { scene_node->data.renderable = renderable; });
}

SharedPtr<Renderable> C_Transform::renderable() const {
    if (node.is<LargeSceneNodeR*>()) {
        return node.get_unchecked<LargeSceneNodeR*>()->data.renderable;
    }
    return node.get_unchecked<SceneNodeR*>()->data.renderable;
}
}  // namespace dw
