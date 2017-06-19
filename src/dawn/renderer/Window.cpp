/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "Window.h"

namespace dw {
Window::Window(Context* context, u16 width, u16 height, const String& title) : Object{context} {
    // Initialise GLFW.
    if (!glfwInit()) {
        // TODO: report error correctly.
        throw Exception{"Failed to initialise GLFW."};
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create the window.
    window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}

Window::~Window() {
    glfwDestroyWindow(window_);
    glfwTerminate();
}

void Window::pollEvents() const {
    glfwPollEvents();
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(window_) != 0;
}
}  // namespace dw