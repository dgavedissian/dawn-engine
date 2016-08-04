/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "renderer/Renderer.h"

namespace dw {

Renderer::Renderer(Context *context) : Object(context) {
    mWindow = glfwCreateWindow(1024, 768, "Dawn Engine", nullptr, nullptr);
}

Renderer::~Renderer() {
    glfwDestroyWindow(mWindow);
}
}
