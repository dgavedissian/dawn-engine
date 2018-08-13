/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Renderer.h"
#include "renderer/Renderable.h"
#include "renderer/Node.h"
#include "scene/SceneManager.h"
#include "scene/CTransform.h"
#include "scene/PhysicsScene.h"
#include "scene/CLinearMotion.h"
#include "net/CNetTransform.h"

namespace dw {
Renderer::Renderer(Context* ctx) : Module(ctx) {
    rhi_ = makeUnique<rhi::Renderer>(ctx);
}

bool Renderer::frame() const {
    return rhi_->frame();
}

rhi::Renderer* Renderer::rhi() const {
    return rhi_.get();
}
}  // namespace dw
