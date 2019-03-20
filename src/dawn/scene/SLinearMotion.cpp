/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "scene/SLinearMotion.h"

namespace dw {
void SLinearMotion::process(SceneManager* scene_mgr, float dt) {
    entityView(scene_mgr).each([dt](auto entity, const auto& linear_motion, auto& node) {
        node.transform().position += linear_motion.velocity * dt;
    });
}
}  // namespace dw
