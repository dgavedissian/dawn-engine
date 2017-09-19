/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "scene/Transform.h"

namespace dw {
Transform::Transform(Context *context, const Position &p, const Quat &o) : Component(context), position_{p}, orientation_{o} {
}

Position &Transform::position() {
    return position_;
}

const Position &Transform::position() const {
    return position_;
}

Quat& Transform::orientation() {
    return orientation_;
}

const Quat& Transform::orientation() const {
    return orientation_;
}
}  // namespace dw
