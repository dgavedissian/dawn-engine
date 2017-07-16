/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "ecs/Component.h"
#include "math/Defs.h"
#include "scene/Position.h"

namespace dw {
struct Transform : public Component {
    Transform(Position position, Quat orientation) : position{position}, orientation{orientation} {
    }

    Position position;
    Quat orientation;
};
}  // namespace dw
