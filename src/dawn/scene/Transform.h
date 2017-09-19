/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "ecs/Component.h"
#include "math/Defs.h"
#include "scene/Position.h"

namespace dw {
class Transform : public Component {
public:
    DW_OBJECT(Transform);

    Transform(Context* context, const Position& p, const Quat& o);

    Position& position();
    const Position& position() const;
    Quat& orientation();
    const Quat& orientation() const;

private:
    Position position_;
    Quat orientation_;

};
}  // namespace dw
