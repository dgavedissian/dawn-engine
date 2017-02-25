/*
* Dawn Engine
* Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
*/
#pragma once

#include "renderer/Material.h"

namespace dw {
class DW_API Geometry {
public:
    Geometry() = default;
    virtual ~Geometry() = default;

    virtual void draw(SharedPtr<Material> material) = 0;
};
}