/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "RHIRenderer.h"

namespace dw {
namespace rhi {
class DW_API NullRenderContext : public RenderContext {
public:
    DW_OBJECT(NullRenderContext);

    NullRenderContext(Context* ctx);
    virtual ~NullRenderContext();

    // Window management. Executed on the main thread.
    Result<void> createWindow(u16 width, u16 height, const String& title) override;
    void destroyWindow() override;
    void processEvents() override;
    bool isWindowClosed() const override;
    Vec2i windowSize() const override;
    Vec2 windowScale() const override;
    Vec2i backbufferSize() const override;

    // Command buffer processing. Executed on the render thread.
    void startRendering() override;
    void stopRendering() override;
    void processCommandList(Vector<RenderCommand>& command_list) override;
    bool frame(const Frame* frame) override;
};
}  // namespace rhi
}  // namespace dw
