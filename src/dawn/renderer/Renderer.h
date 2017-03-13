/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/RenderTask.h"
#include "ecs/System.h"

namespace dw {
class DW_API Renderer : public System {
public:
    DW_OBJECT(Renderer);

    Renderer(Context* context);
    ~Renderer();

    /// Render a single frame.
    void frame();

    void processEntity(Entity& entity) override;

private:
    uint width_, height_;

    Vector<RenderTask> render_tasks_;

    // O(1) add, O(N) removal. Optimised for iteration speed.
    // Vector<Renderable*> render_queue_;
    // friend class Renderable;
    // void AddToRenderQueue(Renderable* renderable);
    // void RemoveFromRenderQueue(Renderable* renderable);
};
}
