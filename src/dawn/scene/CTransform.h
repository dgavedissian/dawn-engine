/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "Component.h"
#include "core/io/InputStream.h"
#include "core/io/OutputStream.h"
#include "core/math/Defs.h"
#include "core/scene/SceneNode.h"
#include "renderer/SceneNode.h"

namespace dw {
class DW_API Renderable;

struct DW_API CTransform : public Component {
    CTransform(LargeSceneNodeR* scene_node);
    CTransform(LargeSceneNodeR* scene_node, SharedPtr<Renderable> renderable);
    CTransform(SceneNodeR* scene_node);
    CTransform(SceneNodeR* scene_node, SharedPtr<Renderable> renderable);

    void attachTo(CTransform* new_parent);

    bool isLargeTransform() const;

    LargeSceneNodeR& largeNode();
    SceneNodeR& node();

    Variant<LargeSceneNodeR*, SceneNodeR*> scene_node;

    void setRenderable(SharedPtr<Renderable> renderable);
    SharedPtr<Renderable> renderable() const;

    // Helper functions
    void move(const Vec3& offset);
    Quat& orientation();
    const Quat& orientation() const;
};
}  // namespace dw
