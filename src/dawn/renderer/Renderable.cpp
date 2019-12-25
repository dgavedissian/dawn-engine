/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "renderer/Renderable.h"

namespace dw {
Renderable::Renderable() : material_(nullptr), render_queue_group_(RenderQueueGroup::Main) {
}

void Renderable::setMaterial(SharedPtr<Material> material) {
    material_ = std::move(material);
}

void Renderable::setRenderQueueGroup(RenderQueueGroup render_queue_group) {
    render_queue_group_ = render_queue_group;
}

Material* Renderable::material() const {
    return material_.get();
}

RenderQueueGroup Renderable::renderQueueGroup() const {
    return render_queue_group_;
}
}  // namespace dw
