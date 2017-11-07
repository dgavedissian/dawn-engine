/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "scene/Transform.h"
#include "renderer/Renderable.h"

namespace dw {

/// Used to keep track of the current universe, which contains all entities.
class DW_API Universe : public Object {
public:
    DW_OBJECT(Universe);

    Universe(Context* context);
    ~Universe();

    /// Set up star system.
    void createStarSystem();

    /// Calls update on each entity
    /// @param dt Time elapsed
    void update(float dt);

    /// Returns the root node in the scene graph.
    Transform* rootNode() const;

private:
    SharedPtr<Transform> root_node_;

    SharedPtr<RenderableNode> background_renderable_root_;
    Entity* background_entity_;
};
}  // namespace dw
