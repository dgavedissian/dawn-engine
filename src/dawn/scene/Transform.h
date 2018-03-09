/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "Component.h"
#include "math/Defs.h"
#include "scene/Position.h"
#include "io/InputStream.h"
#include "io/OutputStream.h"

namespace dw {
class Transform : public Component {
public:
    Transform(const Position& p, const Quat& o);
    Transform(const Position& p, const Quat& o, Entity& parent_entity);
    Transform(const Position& p, const Quat& o, Transform* parent);

    void setRelativeToCamera(bool relative_to_camera);

    Position& position();
    const Position& position() const;
    Quat& orientation();
    const Quat& orientation() const;

    /// Model matrices only make sense if they're relative to a position.
    Mat4 modelMatrix(const Position& camera_position) const;

    Transform* parent();

    void attachTo(Transform* new_parent);

private:
    Position position_;
    Quat orientation_;

    bool relative_to_camera_;
    Transform* parent_;
    Vector<Transform*> children_;
};
}  // namespace dw
