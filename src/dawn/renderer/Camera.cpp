/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Camera.h"

namespace dw {
Camera::Camera(float near, float far, float fov_y, float aspect)
    : near{near}, far{far}, fov{fov}, aspect{aspect} {
    float tangent = static_cast<float>(tan(fov_y * dw::M_DEGTORAD_OVER_2));  // tangent of half fovY
    float v = near * tangent * 2;  // half height of near plane
    float h = v * aspect;          // half width of near plane
    projection_matrix = Mat4::OpenGLPerspProjRH(near, far, h, v);
}
}  // namespace dw