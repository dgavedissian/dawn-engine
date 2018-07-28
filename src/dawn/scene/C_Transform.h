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

struct DW_API C_Transform : public Component {
    C_Transform(LargeSceneNodeR* scene_node);
    C_Transform(LargeSceneNodeR* scene_node, SharedPtr<Renderable> renderable);
    C_Transform(SceneNodeR* scene_node);
    C_Transform(SceneNodeR* scene_node, SharedPtr<Renderable> renderable);

    void attachTo(C_Transform* new_parent);

    // Transform.
    LargePosition position;
    Quat orientation;

    Variant<LargeSceneNodeR*, SceneNodeR*> node;

    void setRenderable(SharedPtr<Renderable> renderable);
    SharedPtr<Renderable> renderable() const;
};
}  // namespace dw
