/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "core/math/Defs.h"

namespace dw {
template <typename T>
class SceneGraph
{
public:
	SceneGraph();
	~SceneGraph();
};
}  // namespace dw

#include "core/scene/SceneGraph.i.h"
