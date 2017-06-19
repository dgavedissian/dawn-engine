/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/GL.h"
#include "renderer/GLFW.h"

namespace dw {
class Window : public Object {
public:
    DW_OBJECT(Window);

    Window(Context* context, u16 width, u16 height, const String& title);
    ~Window();

    void pollEvents() const;
    bool shouldClose() const;

    friend class Renderer;

private:
    GLFWwindow* window_;
};
}  // namespace dw