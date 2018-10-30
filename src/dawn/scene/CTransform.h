/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "Component.h"
#include "core/math/Defs.h"
#include "renderer/Node.h"

namespace dw {
class DW_API Renderable;

struct DW_API CTransform : public Component {
    CTransform(Node* scene_node);
    CTransform(Node* scene_node, SharedPtr<Renderable> renderable);

    void attachTo(CTransform* new_parent);

    Node* node;

    void setRenderable(SharedPtr<Renderable> renderable);
    SharedPtr<Renderable> renderable() const;
};
}  // namespace dw
