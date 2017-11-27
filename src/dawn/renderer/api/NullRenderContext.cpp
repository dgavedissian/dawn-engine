/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/api/NullRenderContext.h"

namespace dw {
NullRenderContext::NullRenderContext(Context* ctx) : RenderContext(ctx) {
}

NullRenderContext::~NullRenderContext() {
}

void NullRenderContext::createWindow(u16, u16, const String&) {
}

void NullRenderContext::destroyWindow() {
}

void NullRenderContext::processEvents() {
}

bool NullRenderContext::isWindowClosed() const {
    return false;
}

void NullRenderContext::startRendering() {
}

void NullRenderContext::stopRendering() {
}

void NullRenderContext::processCommandList(Vector<RenderCommand>&) {
}

bool NullRenderContext::frame(const Frame*) {
    return true;
}

}  // namespace dw