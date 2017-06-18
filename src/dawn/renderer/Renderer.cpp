/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Renderer.h"

namespace dw {
Renderer::Renderer(Context* context, Window* window)
    : Object{context}, window_{window->window_}, should_exit_{false} {
    render_thread_ = Thread{[this]() { renderThread(); }};
}

Renderer::~Renderer() {
    should_exit_ = true;
    render_thread_.join();
}

void Renderer::pushRenderTask(RenderTask&& task) {
    // render_tasks_.emplace_back(task);
}

void Renderer::frame() {
    /*
    bgfx::setViewRect(0, 0, 0, width_, height_);
    for (auto task : render_tasks_) {
        // Set camera state.
        if (task.type == RenderTaskType::SetCameraMatrices) {
            auto& task_data = task.camera;
            task_data.view_matrix.Transpose();
            task_data.proj_matrix.Transpose();
            bgfx::setViewTransform(0, &task_data.view_matrix.v[0][0],
                                   &task_data.proj_matrix.v[0][0]);
        }
        // Render.
        if (task.type == RenderTaskType::Primitive) {
            auto& task_data = task.primitive;
            task_data.model_matrix.Transpose();
            bgfx::setTransform(&task_data.model_matrix.v[0][0]);
            bgfx::setVertexBuffer(task_data.vb);
            bgfx::setIndexBuffer(task_data.ib);
            bgfx::submit(0, task_data.shader);
        }
    }
    render_tasks_.clear();
    bgfx::frame();
     */
}

void Renderer::renderThread() {
    glfwMakeContextCurrent(window_);

    // Enter event loop.
    while (!should_exit_) {
        // bgfx::renderFrame();
        glfwSwapBuffers(window_);
    }
}
}  // namespace dw