/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "renderer/Renderer.h"

namespace dw {
class Renderer::RendererLoggerImpl : public gfx::Logger {
public:
    explicit RendererLoggerImpl(dw::Logger& logger) : logger_{logger} {
    }

    void log(gfx::LogLevel level, const std::string& value) override {
        LogLevel converted_level;
        switch (level) {
            case gfx::LogLevel::Debug:
                converted_level = LogLevel::Debug;
                break;
            case gfx::LogLevel::Info:
                converted_level = LogLevel::Info;
                break;
            case gfx::LogLevel::Warning:
                converted_level = LogLevel::Warning;
                break;
            case gfx::LogLevel::Error:
                converted_level = LogLevel::Error;
                break;
            default:
                converted_level = LogLevel::Info;
        }
        logger_.log(converted_level, value);
    }

private:
    dw::Logger& logger_;
};

Renderer::Renderer(Context* ctx)
    : Module(ctx),
      frame_time_(0.0f),
      frames_per_second_(0),
      frame_counter_(0),
      last_fps_update_(time::beginTiming()) {
    renderer_logger_ = makeUnique<RendererLoggerImpl>(log());
    renderer_ = makeUnique<gfx::Renderer>(*renderer_logger_);
}

bool Renderer::frame() {
    auto result = renderer_->frame();

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

gfx::Renderer* Renderer::rhi() const {
    return renderer_.get();
}

double Renderer::frameTime() const {
    return frame_time_;
}

int Renderer::framesPerSecond() const {
    return frames_per_second_;
}
}  // namespace dw
