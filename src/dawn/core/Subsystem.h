/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "core/Object.h"

namespace dw {
class DW_API Subsystem : public Object {
public:
    DW_OBJECT(Subsystem)

    Subsystem(Context* ctx);
    virtual ~Subsystem();
};
}  // namespace dw