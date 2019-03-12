/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "renderer/Renderer.h"
#include "scene/SceneManager.h"

namespace dw {
Renderer::Renderer(Context* ctx)
    : Module(ctx),
      frame_time_(0.0f),
      frames_per_second_(0),
      frame_counter_(0),
      last_fps_update_(time::beginTiming()) {
    rhi_ = makeUnique<rhi::RHIRenderer>(ctx);
}

bool Renderer::frame() {
    auto result = rhi_->frame();

    // Update frame counter.
    frame_counter_++;
    time::TimePoint current_time = time::beginTiming();
    if (time::elapsed(last_fps_update_, current_time) > 1.0) {
        last_fps_update_ = current_time;
        frames_per_second_ = frame_counter_;
        frame_counter_ = 0;
    }

    return result;
}

rhi::RHIRenderer* Renderer::rhi() const {
    return rhi_.get();
}

double Renderer::frameTime() const {
    return frame_time_;
}

int Renderer::framesPerSecond() const {
    return frames_per_second_;
}
}  // namespace dw
