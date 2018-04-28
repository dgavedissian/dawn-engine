/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
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

Vec2i NullRenderContext::windowSize() const {
    return {0, 0};
}

Vec2 NullRenderContext::windowScale() const {
    return {1.0f, 1.0f};
}

Vec2i NullRenderContext::backbufferSize() const {
    return {0, 0};
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