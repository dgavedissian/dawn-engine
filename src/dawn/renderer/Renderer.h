/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/RenderTask.h"

namespace dw {
class DW_API Renderer : public Object {
public:
    DW_OBJECT(Renderer);

    Renderer(Context* context);
    ~Renderer() = default;

    /// Push render task.
    void pushRenderTask(RenderTask&& task);

    /// Render a single frame.
    void frame();

private:
    u16 width_, height_;
    Vector<RenderTask> render_tasks_;
};
}
