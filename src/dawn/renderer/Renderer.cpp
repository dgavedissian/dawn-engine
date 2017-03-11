/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "ecs/System.h"
#include "renderer/Renderer.h"
#include "renderer/Renderable.h"
#include "scene/Transform.h"
#include "scene/Parent.h"

namespace dw {
Renderer::Renderer(Context* context) : System{context}, width_{1280}, height_{800} {
    supportsComponents<Renderable, Transform>();

    bgfx::init(bgfx::RendererType::OpenGL);
    bgfx::reset(width_, height_, BGFX_RESET_NONE);

    // Set view 0 clear state.
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
}

Renderer::~Renderer() {
}

void Renderer::frame() {
    bgfx::setViewRect(0, 0, 0, width_, height_);
    for (auto task : render_tasks_) {
        // Render.
        bgfx::setVertexBuffer(task.vb);
        bgfx::setIndexBuffer(task.ib);
        bgfx::submit(0, task.shader);
    }
    bgfx::frame();
}

void Renderer::processEntity(Entity& entity) {
    auto renderable = entity.component<Renderable>();
    auto transform = entity.component<Transform>();
    auto parent = entity.component<Parent>();

    /*Vec3 relative_position = PositionData::WorldToRelative(current_camera, transform.position);
    Mat4 model = CalcModelMatrix(relative_position, transform.orientation);
    if (parent) {
        model = model * DeriveWorldTransform(parent.entity);
    }
    */
    render_tasks_.emplace_back(renderable->draw(Mat4()));
}

// void Renderer::AddToRenderQueue(Renderable* renderable) {
//    render_queue_.emplace_back(renderable);
//}
//
// void Renderer::RemoveFromRenderQueue(Renderable* renderable) {
//    auto iterator = std::find(render_queue_.begin(), render_queue_.end(), renderable);
//    if (iterator != render_queue_.end()) {
//        render_queue_.erase(iterator);
//    } else {
//        log().warn("Attempted to remove a Renderable object from the render queue which wasn't
//        in the queue.");
//    }
//}
}
