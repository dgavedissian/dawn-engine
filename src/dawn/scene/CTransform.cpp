/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "scene/CTransform.h"

namespace dw {
CTransform::CTransform(LargeSceneNodeR* scene_node) : scene_node(scene_node) {
}

CTransform::CTransform(LargeSceneNodeR* scene_node, SharedPtr<Renderable> renderable)
    : scene_node(scene_node) {
    setRenderable(renderable);
}

CTransform::CTransform(SceneNodeR* scene_node) : scene_node(scene_node) {
}

CTransform::CTransform(SceneNodeR* scene_node, SharedPtr<Renderable> renderable)
    : scene_node(scene_node) {
    setRenderable(renderable);
}

void CTransform::attachTo(CTransform* new_parent) {
    scene_node.match(
        [new_parent](LargeSceneNodeR* scene_node) {
            new_parent->scene_node.get<LargeSceneNodeR*>()->addChild(scene_node);
        },
        [new_parent](SceneNodeR* scene_node) {
            new_parent->scene_node.match(
                [scene_node](LargeSceneNodeR* parent_scene_node) {
                    parent_scene_node->addChild(scene_node);
                },
                [scene_node](SceneNodeR* parent_scene_node) {
                    parent_scene_node->addChild(scene_node);
                });
        });
}

bool CTransform::isLargeTransform() const {
    return scene_node.is<LargeSceneNodeR*>();
}

LargeSceneNodeR& CTransform::largeNode() {
    return *scene_node.get<LargeSceneNodeR*>();
}

SceneNodeR& CTransform::node()
{
    return *scene_node.get<SceneNodeR*>();
}

void CTransform::setRenderable(SharedPtr<Renderable> renderable) {
    scene_node.match(
        [renderable](LargeSceneNodeR* scene_node) { scene_node->data.renderable = renderable; },
        [renderable](SceneNodeR* scene_node) { scene_node->data.renderable = renderable; });
}

SharedPtr<Renderable> CTransform::renderable() const {
    if (scene_node.is<LargeSceneNodeR*>()) {
        return scene_node.get_unchecked<LargeSceneNodeR*>()->data.renderable;
    }
    return scene_node.get_unchecked<SceneNodeR*>()->data.renderable;
}

void CTransform::move(const Vec3& offset) {
    if (scene_node.is<LargeSceneNodeR*>()) {
        scene_node.get_unchecked<LargeSceneNodeR*>()->position += offset;
    }
    else {
        scene_node.get_unchecked<SceneNodeR*>()->transform().position += offset;
    }
}

Quat& CTransform::orientation() {
    if (scene_node.is<LargeSceneNodeR*>()) {
        return scene_node.get_unchecked<LargeSceneNodeR*>()->orientation;
    }
    return scene_node.get_unchecked<SceneNodeR*>()->transform().orientation;
}

const Quat& CTransform::orientation() const {
    if (scene_node.is<LargeSceneNodeR*>()) {
        return scene_node.get_unchecked<LargeSceneNodeR*>()->orientation;
    }
    return scene_node.get_unchecked<SceneNodeR*>()->transform().orientation;
}
}  // namespace dw
