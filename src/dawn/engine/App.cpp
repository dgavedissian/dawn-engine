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

    int returnCode = EXIT_SUCCESS;

    dw::Engine engine{app->gameName(), app->gameVersion()};
    engine.setup();

    // App lifecycle.
    app->context_ = engine.context();
    app->engine_ = &engine;
    app->init(argc, argv);
    engine.run([&app](float dt) { app->update(dt); }, [&app] { app->render(); });
    app->shutdown();
    app.reset();

    engine.shutdown();

    return returnCode;
}
}  // namespace dw
