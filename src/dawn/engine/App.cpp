/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "engine/App.h"
#include "engine/Engine.h"

namespace dw {

int runApp(UniquePtr<App> app, int argc, char** argv) {
    // TODO(David) Load config.

    dw::Engine engine{app->gameName(), app->gameVersion()};
    engine.setup(argc, argv);

    // App lifecycle.
    app->context_ = engine.context();
    app->engine_ = &engine;
    app->init(argc, argv);
    engine.run([&app](float dt) { app->update(dt); },
               [&app](float interpolation) { app->render(interpolation); });
    app->shutdown();
    app.reset();

    engine.shutdown();

    return EXIT_SUCCESS;
}
}  // namespace dw
