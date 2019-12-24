/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "scene/SLinearMotion.h"

namespace dw {
void SLinearMotion::process(float dt) {
    entityView().each([dt](auto entity, const auto& linear_motion, auto& node) {
        node.transform().position += linear_motion.velocity * dt;
    });
}
}  // namespace dw
