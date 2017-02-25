/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Renderer.h"

namespace dw {

Renderer::Renderer(Context* context) : Object(context), width_(1280), height_(800) {
    bgfx::init(bgfx::RendererType::OpenGL);
    bgfx::reset(width_, height_, BGFX_RESET_NONE);

    // Set view 0 clear state.
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
}

Renderer::~Renderer() {
}

Node* Renderer::GetRootNode() const {
    return root_node_.get();
}

void Renderer::Frame() {
    bgfx::setViewRect(0, 0, 0, width_, height_);
    for (auto renderable : render_queue_) {
        renderable->Draw();
    }
    bgfx::frame();
}

void Renderer::AddToRenderQueue(Renderable* renderable) {
    render_queue_.emplace_back(renderable);
}

void Renderer::RemoveFromRenderQueue(Renderable* renderable) {
    auto iterator = std::find(render_queue_.begin(), render_queue_.end(), renderable);
    if (iterator != render_queue_.end()) {
        render_queue_.erase(iterator);
    } else {
        getLog().warn("Attempted to remove a Renderable object from the render queue which wasn't in the queue.");
    }
}
}
