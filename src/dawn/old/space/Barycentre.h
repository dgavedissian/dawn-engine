/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#pragma once

#include "SystemBody.h"

namespace dw {

// A body that only serves as an empty point where other bodies orbit around
class DW_API Barycentre : public SystemBody {
public:
    Barycentre(Renderer* rs);
    virtual ~Barycentre();
};
}  // namespace dw
