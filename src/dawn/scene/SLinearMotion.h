/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#pragma once

#include "scene/CLinearMotion.h"
#include "scene/EntitySystem.h"

namespace dw {
class SLinearMotion : public EntitySystem {
public:
    DW_OBJECT(SLinearMotion);

    explicit SLinearMotion(Context* ctx);
    void processEntity(Entity& e, float dt) override;
};
}  // namespace dw
