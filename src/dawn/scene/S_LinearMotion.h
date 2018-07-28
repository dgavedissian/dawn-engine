/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "scene/C_LinearMotion.h"
#include "scene/System.h"

namespace dw {
class S_LinearMotion : public System {
public:
    DW_OBJECT(S_LinearMotion);

    explicit S_LinearMotion(Context* ctx);
    void processEntity(Entity& e, float dt) override;
};
}  // namespace dw
