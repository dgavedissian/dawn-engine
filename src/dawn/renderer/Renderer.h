/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "renderer/rhi/RHIRenderer.h"

namespace dw {
class DW_API Renderer : public Module {
public:
    DW_OBJECT(Renderer);

    Renderer(Context* ctx);
    ~Renderer() = default;

    /// Render a single frame.
    bool frame() const;

    /// Get the renderer hardware interface.
    rhi::RHIRenderer* rhi() const;

private:
    UniquePtr<rhi::RHIRenderer> rhi_;
};
}  // namespace dw
