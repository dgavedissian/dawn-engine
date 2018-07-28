/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "core/scene/SceneNode.h"

namespace dw {
namespace detail {
Mat4 Transform::calculateModelMatrix() const {
    return Mat4::Translate(position).ToFloat4x4() * Mat4::FromQuat(orientation);
    ;
}
}  // namespace detail
}  // namespace dw
