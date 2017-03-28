/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Renderer.h"

namespace dw {
Renderer::Renderer(Context* context) : Object{context}, width_{1280}, height_{800} {
    bgfx::init(bgfx::RendererType::OpenGL);
    bgfx::reset(width_, height_, BGFX_RESET_NONE);

    // Set view 0 clear state.
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
}

void Renderer::pushRenderTask(RenderTask&& task) {
    render_tasks_.emplace_back(task);
}

void Renderer::frame() {
    bgfx::setViewRect(0, 0, 0, width_, height_);
    for (auto task : render_tasks_) {
        // Render.
        bgfx::setVertexBuffer(task.vb);
        bgfx::setIndexBuffer(task.ib);
        bgfx::submit(0, task.shader);
    }
    render_tasks_.clear();
    bgfx::frame();
}
}