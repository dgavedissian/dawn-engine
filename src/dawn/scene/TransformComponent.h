/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "Component.h"
#include "core/io/InputStream.h"
#include "core/io/OutputStream.h"
#include "core/math/Defs.h"
#include "core/scene/LargePosition.h"

namespace dw {
class TransformComponent : public Component {
public:
    TransformComponent(const LargePosition& p, const Quat& o);
    TransformComponent(const LargePosition& p, const Quat& o, TransformComponent* parent);

    void setRelativeToCamera(bool relative_to_camera);

    /// Model matrices only make sense if they're relative to a position.
    Mat4 modelMatrix(const LargePosition& camera_position) const;

    TransformComponent* parent() const;

    void attachTo(TransformComponent* new_parent);

    // Transform.
    LargePosition position;
    Quat orientation;

private:
    bool relative_to_camera_;
    TransformComponent* parent_;
    Vector<TransformComponent*> children_;
};
}  // namespace dw
