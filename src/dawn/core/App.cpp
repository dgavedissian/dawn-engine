/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "App.h"
#include "Engine.h"

namespace dw {
int runApp(UniquePtr<App> app, int argc, char** argv) {
    // TODO(David) Load config.

    CommandLine cmdline(argc, argv);

    dw::Engine engine{app->gameName(), app->gameVersion()};
    engine.setup(cmdline);

    // App lifecycle.
    app->context_ = engine.context();
    app->engine_ = &engine;
    app->init(cmdline);
    engine.run([&app](float dt) { app->update(dt); },
               [&app](float interpolation) { app->render(interpolation); });
    app->shutdown();
    app.reset();

    engine.shutdown();

    return EXIT_SUCCESS;
}
}  // namespace dw
