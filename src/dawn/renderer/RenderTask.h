/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "math/Defs.h"
#include "renderer/Renderer.h"

namespace dw {
enum class RenderTaskType { SetCameraMatrices, Primitive };

struct RenderTask {
    RenderTaskType type;
    struct {
        Mat4 view_matrix;
        Mat4 proj_matrix;
    } camera;
    struct {
        Mat4 model_matrix;
        u32 count;
        VertexBufferHandle vb;
        IndexBufferHandle ib;
        ProgramHandle shader;
    } primitive;
};
}  // namespace dw
