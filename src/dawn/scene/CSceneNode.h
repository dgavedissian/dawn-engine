/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#pragma once

#include "Component.h"
#include "core/math/Defs.h"
#include "renderer/Node.h"

namespace dw {
class DW_API Renderable;

struct DW_API CSceneNode : public Component {
    CSceneNode(Node* scene_node);
    CSceneNode(Node* scene_node, SharedPtr<Renderable> renderable);

    void attachTo(CSceneNode* new_parent);

    Node* node;

    // Helper functions.
    detail::Transform& transform();
    const detail::Transform& transform() const;

    void setRenderable(SharedPtr<Renderable> renderable);
    SharedPtr<Renderable> renderable() const;
};
}  // namespace dw
