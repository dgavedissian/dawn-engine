/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "Base.h"

namespace dw {
enum class SpectralClass { O, B, A, F, G, K, M };

struct DW_API StarDesc {
    float radius = 0.0f;
    SpectralClass spectralClass = SpectralClass::G;

    StarDesc() = default;
};
}
