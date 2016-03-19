/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "DawnEngine.h"
#include "SandboxState.h"

int main(int argc, char** argv)
{
	// Set up the engine
    dw::Engine* engine = new dw::Engine("Sandbox", "1.0.0");
	engine->Setup();

	// Add resource locations
	// TODO: move this out of the renderer class
	engine->GetRenderer()->AddResourceLocation(engine->GetBasePath() + "media/sandbox");

	// Register the sandbox state and switch to it
	engine->GetStateMgr()->RegisterState(dw::MakeShared<SandboxState>(engine));
	engine->GetStateMgr()->Switch(S_SANDBOX);

	// Run the engine
	engine->Run([](float) {});

	// Shutdown and exit
    engine->Shutdown();
    return 0;
}
