/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "DawnEngine.h"
#include "SandboxState.h"

int main(int argc, char** argv) {
    // Set up the engine
    dw::Engine* engine = new dw::Engine("Sandbox", "1.0.0");
    engine->setup();

    // Add resource locations
    // TODO: move this out of the renderer class
    engine->getRenderer()->addResourceLocation(engine->getBasePath() + "media/sandbox");

    // Register the sandbox state and switch to it
    engine->getStateMgr()->registerState(dw::makeShared<SandboxState>(engine));
    engine->getStateMgr()->changeTo(S_SANDBOX);

    // Run the engine
    engine->run([](float) {});

    // Shutdown and exit
    engine->shutdown();
    return 0;
}
