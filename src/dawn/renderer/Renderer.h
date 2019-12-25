/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "core/Timer.h"

#include <dawn-gfx/Renderer.h>

namespace dw {
class DW_API Renderer : public Module {
public:
    DW_OBJECT(Renderer);

    explicit Renderer(Context* ctx);
    ~Renderer() override = default;

    /// Render a single frame.
    bool frame();

    /// Get the renderer hardware interface.
    gfx::Renderer* gfx() const;

    /// Access the last frame time
    double frameTime() const;

    /// Access the frames per second metric.
    int framesPerSecond() const;

private:
    class RendererLoggerImpl;

    UniquePtr<gfx::Renderer> renderer_;
    SharedPtr<RendererLoggerImpl> renderer_logger_;  // needs to be shared as type is not defined.

    double frame_time_;
    int frames_per_second_;
    int frame_counter_;
    time::TimePoint last_fps_update_;
};
}  // namespace dw
