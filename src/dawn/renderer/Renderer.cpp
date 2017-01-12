/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Renderer.h"

namespace dw {

Renderer::Renderer(Context* context) : Object(context), mWidth(1280), mHeight(800) {
    bgfx::init(bgfx::RendererType::OpenGL);
    bgfx::reset(mWidth, mHeight, BGFX_RESET_NONE);

    // Set view 0 clear state.
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
}

Renderer::~Renderer() {
}

void Renderer::frame() {
    bgfx::setViewRect(0, 0, 0, mWidth, mHeight);
    bgfx::touch(0);

    bgfx::frame();
}
}
