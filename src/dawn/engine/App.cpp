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

int runApp(UniquePtr<App> app, int argc, char** argv) {
    // TODO(David) Load config
    // TODO(David) Move config into the main thread

    // Open window
    if (!glfwInit())
        return EXIT_FAILURE;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, app->gameName().c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Bind bgfx to glfw, and prevent bgfx from spawning a render thread
    bgfx::glfwSetWindow(window);
    bgfx::renderFrame();

    // Launch main thread
    Thread mainThread(
        [argc, argv](UniquePtr<App> app) {
            dw::Engine engine{app->gameName(), app->gameVersion()};
            engine.setup();

            // App lifecycle
            app->context_ = engine.context();
            app->init(argc, argv);
            engine.run([&app](float dt) { app->update(dt); });
            app->shutdown();
            app.reset();

            engine.shutdown();
        },
        std::move(app));

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
