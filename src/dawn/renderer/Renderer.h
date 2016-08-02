/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include <glfw/glfw3.h>

namespace dw {

class Renderer : public Object {
public:
    DW_OBJECT(Renderer);

    Renderer(Context* context);
    ~Renderer();

private:
    GLFWwindow* mWindow;
};

}
