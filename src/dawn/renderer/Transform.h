/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "scene/Component.h"
#include "core/io/InputStream.h"
#include "core/io/OutputStream.h"
#include "core/math/Defs.h"
#include "core/Position.h"

namespace dw {
class LocalTransform;

class Transform {
public:
    Transform(const Position& p, const Quat& o, const Vec3& s);

    void setRelativeToCamera(bool relative_to_camera);

    /// Model matrices only make sense if they're relative to a position.
    Mat4 modelMatrix(const Position& camera_position) const;

    Transform* parent() const;

    void addChild(Transform* child);
    void addChild(LocalTransform* child);

    // Transform data.
    Position position;
    Quat orientation;
    Vec3 scale;

private:
    bool relative_to_camera_;
    Transform* parent_;
    Vector<Transform*> children_;
    Vector<LocalTransform*> lt_children_;
    byte depth_;

    void detachFromParent();

    friend class LocalTransform;
};

class LocalTransform {
public:
    LocalTransform(const Vec3& p, const Quat& o, const Vec3& s);

    Vec3 position;
    Quat orientation;
    Vec3 scale;

    void addChild(LocalTransform* child);

private:
    LocalTransform* lt_parent_;
    Transform* t_parent_;
    Vector<LocalTransform*> children_;
    byte depth_;

    void detachFromParent();

    friend class Transform;
};
}  // namespace dw
