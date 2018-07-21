/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "scene/Component.h"
#include "core/io/InputStream.h"
#include "core/io/OutputStream.h"
#include "core/math/Defs.h"
#include "core/scene/LargePosition.h"

namespace dw {
class Transform;

class LargeTransform {
public:
    LargeTransform(const LargePosition& p, const Quat& o, const Vec3& s);

    void setRelativeToCamera(bool relative_to_camera);

    /// Model matrices only make sense if they're relative to a position.
    Mat4 modelMatrix(const LargePosition& camera_position) const;

    LargeTransform* parent() const;

    void addChild(LargeTransform* child);
    void addChild(Transform* child);

    // Transform data.
    LargePosition position;
    Quat orientation;
    Vec3 scale;

private:
    bool relative_to_camera_;
    LargeTransform* parent_;
    Vector<LargeTransform*> children_;
    Vector<Transform*> lt_children_;
    byte depth_;

    void detachFromParent();

    friend class Transform;
};

class Transform {
public:
    Transform(const Vec3& p, const Quat& o, const Vec3& s);

    Vec3 position;
    Quat orientation;
    Vec3 scale;

    void addChild(Transform* child);

private:
    Transform* lt_parent_;
    LargeTransform* t_parent_;
    Vector<Transform*> children_;
    byte depth_;

    void detachFromParent();

    friend class LargeTransform;
};
}  // namespace dw
