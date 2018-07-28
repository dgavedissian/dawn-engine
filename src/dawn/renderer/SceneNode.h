/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "core/scene/SceneNode.h"

namespace dw {
class DW_API Renderable;

namespace detail {
struct RendererSceneNodeData {
    SharedPtr<Renderable> renderable;
};
}  // namespace detail

using LargeSceneNodeR = LargeSceneNode<detail::RendererSceneNodeData>;
using SceneNodeR = SceneNode<detail::RendererSceneNodeData>;
}  // namespace dw
