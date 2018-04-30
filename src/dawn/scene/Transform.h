/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "Component.h"
#include "core/io/InputStream.h"
#include "core/io/OutputStream.h"
#include "core/math/Defs.h"
#include "scene/Position.h"

namespace dw {
class Transform : public Component {
public:
    Transform(const Position& p, const Quat& o);
    Transform(const Position& p, const Quat& o, Transform* parent);

    void setRelativeToCamera(bool relative_to_camera);

    /// Model matrices only make sense if they're relative to a position.
    Mat4 modelMatrix(const Position& camera_position) const;

    Transform* parent() const;

    void attachTo(Transform* new_parent);

    // Transform.
    Position position;
    Quat orientation;

private:
    bool relative_to_camera_;
    Transform* parent_;
    Vector<Transform*> children_;
};
}  // namespace dw
