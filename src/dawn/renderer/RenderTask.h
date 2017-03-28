/*
* Dawn Engine
* Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
*/
#pragma once
#include "math/Defs.h"

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
            bgfx::VertexBufferHandle vb;
            bgfx::IndexBufferHandle ib;
            bgfx::ProgramHandle shader;
        } primitive;
};
}
