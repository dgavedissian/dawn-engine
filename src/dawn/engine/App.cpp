/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "engine/App.h"
#include "engine/Engine.h"

#include <bgfx/bgfxplatform.h>

namespace dw {

int returnCode = EXIT_SUCCESS;

// TODO(David) replace this with an event queue from the render thread
bool exit = false;

void mainThreadFunc(App* app, int argc, char** argv) {
    returnCode = runEngine(app, argc, argv);
}

int runApp(App* app, int argc, char** argv) {
    // TODO(David) Load config
    // TODO(David) Move config into the main thread

    // Open window
    if (!glfwInit())
        return EXIT_FAILURE;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, app->getGameName().c_str(), NULL, NULL);
    glfwMakeContextCurrent(window);

    // Bind bgfx to glfw, and prevent bgfx from spawning a render thread
    bgfx::glfwSetWindow(window);
    bgfx::renderFrame();

    // Launch main thread (and ENSURE that app is no longer accessed)
    Thread mainThread(mainThreadFunc, app, argc, argv);

    // Enter event loop
    while (!exit) {
        bgfx::renderFrame();

        glfwPollEvents();
        if (glfwWindowShouldClose(window)) {
            exit = true;
        }
    }

    mainThread.join();

    glfwDestroyWindow(window);
    glfwTerminate();

    // NOTE: This is free from race conditions as long as returnCode is accessed after
    // mainThread.join()
    return returnCode;
}
}
