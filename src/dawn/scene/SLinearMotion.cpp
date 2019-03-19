/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "scene/SLinearMotion.h"

namespace dw {
void SLinearMotion::process(SceneManager* scene_mgr, float dt) {
    for (auto e : view(scene_mgr)) {
        auto entity = Entity{scene_mgr, e};
        entity.transform()->position += entity.component<CLinearMotion>()->velocity * dt;
    }
}
}  // namespace dw
