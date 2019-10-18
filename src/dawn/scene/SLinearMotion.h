/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#pragma once

#include "scene/CLinearMotion.h"
#include "scene/SceneManager.h"

namespace dw {
class SLinearMotion : public EntitySystem<CLinearMotion, CSceneNode> {
public:
    void process(float dt) override;
};
}  // namespace dw
