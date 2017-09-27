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
    Transform(const Position& p, const Quat& o, Entity& entity);
    Transform(const Position& p, const Quat& o, Transform* parent);

    Position& position();
    const Position& position() const;
    Quat& orientation();
    const Quat& orientation() const;
    Mat4 modelMatrix() const;

    Transform* parent();

    void attachTo(Transform* new_parent);

private:
    Position position_;
    Quat orientation_;
    Transform* parent_;
    Vector<Transform*> children_;
};
}  // namespace dw
