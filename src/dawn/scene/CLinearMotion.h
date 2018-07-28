/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "Component.h"
#include "core/math/Defs.h"

namespace dw {
struct CLinearMotion : public Component {
    Vec3 velocity;
};
}  // namespace dw
