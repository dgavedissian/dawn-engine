/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "engine/App.h"

#include <bgfx/bgfxplatform.h>

namespace dw {

int runApp(App* app, int argc, char** argv) {
    // Open window
    if (!glfwInit())
        return EXIT_FAILURE;

    GLFWwindow* window = glfwCreateWindow(1280, 720, app->getGameName().c_str(), NULL, NULL);
    glfwMakeContextCurrent(window);
    bgfx::glfwSetWindow(window);

    // Launch main thread
    // int result = main(_argc, _argv);

    // Enter event loop
    bool exit = false;
    while (!exit) {
        bgfx::renderFrame();

        glfwPollEvents();
        if (glfwWindowShouldClose(window)) {
            exit = true;
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;  // result;
}
}
