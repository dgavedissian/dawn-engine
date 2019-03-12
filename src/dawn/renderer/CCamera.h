/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#pragma once

#include "core/math/Defs.h"
#include "scene/Component.h"

namespace dw {
struct DW_API CCamera : public Component {
    CCamera(float near, float far, float fov_y, float aspect);

    float near;
    float far;
    float fov;
    float aspect;
    Mat4 projection_matrix;
};
}  // namespace dw
