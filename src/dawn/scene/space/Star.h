/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "Base.h"
#include "renderer/Material.h"
#include "scene/space/SystemBody.h"

namespace dw {
enum class SpectralClass { O, B, A, F, G, K, M };

struct DW_API StarDesc {
    float radius = 0.0f;
    SpectralClass spectral_class = SpectralClass::G;

    StarDesc() = default;
};

// A planetary body which can have an atmosphere or ring system.
class StarSystem;
class DW_API Star : public SystemBody {
public:
    DW_OBJECT(Star);

    Star(Context* ctx, SystemNode& system_node, const StarDesc& desc);
    ~Star() override = default;

private:
    StarDesc desc_;
    SharedPtr<Material> surface_material_;
};
}
