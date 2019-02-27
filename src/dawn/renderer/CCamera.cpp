/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "renderer/CCamera.h"

namespace dw {
CCamera::CCamera(float near, float far, float fov_y, float aspect)
    : near{near}, far{far}, fov{fov_y}, aspect{aspect} {
    float tangent = static_cast<float>(tan(fov_y * dw::M_DEGTORAD_OVER_2));  // tangent of half fovY
    float v = near * tangent * 2;  // half height of near plane
    float h = v * aspect;          // half width of near plane
    projection_matrix = Mat4::OpenGLPerspProjRH(near, far, h, v);
}
}  // namespace dw