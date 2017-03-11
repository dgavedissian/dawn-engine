/*
* Dawn Engine
* Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
*/
#pragma once
#include "math/Defs.h"

namespace dw {
struct RenderTask {
    Mat4 model_matrix;
    bgfx::VertexBufferHandle vb;
    bgfx::IndexBufferHandle ib;
    bgfx::ProgramHandle shader;
};
}
