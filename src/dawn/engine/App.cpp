/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "engine/App.h"
#include "engine/Engine.h"

#include <bgfx/bgfxplatform.h>

namespace dw {

static int returnCode = EXIT_SUCCESS;

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

    GLFWwindow* window = glfwCreateWindow(1280, 720, app->getGameName().c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Bind bgfx to glfw, and prevent bgfx from spawning a render thread
    bgfx::glfwSetWindow(window);
    bgfx::renderFrame();

    // Launch main thread
    Thread mainThread([app, argc, argv]() {
        dw::Engine engine{app->getGameName(), app->getGameVersion()};
        engine.setup();

        // App lifecycle
        app->_setContext(engine.getContext());
        app->init(argc, argv);
        engine.run([&app](float dt) { app->update(dt); });
        app->shutdown();
        delete app;

        engine.shutdown();
    });

    // Enter event loop
    bool exit = false;
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
