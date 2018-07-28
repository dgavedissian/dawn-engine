/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "core/math/Defs.h"

#include "core/scene/SceneNode.h"

namespace dw {
template <typename T> class SceneGraph {
public:
    SceneGraph();
    ~SceneGraph();

    LargeSceneNode<T>& root();

    // Root scene node for objects attached to _all_ cameras.
    SceneNode<T>& backgroundNode();

private:
    detail::SceneNodePool<T> pool_;
    LargeSceneNode<T> root_;
    SceneNode<T> background_root_;
};
}  // namespace dw

#include "core/scene/SceneGraph.i.h"
