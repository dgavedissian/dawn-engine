/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "DawnEngine.h"

int main(int argc, char** argv)
{
    dw::Engine* engine = new dw::Engine("Sandbox", "1.0.0");
    engine->Setup();
    dw::DefaultCamera* camera = new dw::DefaultCamera(engine->GetRenderer(), engine->GetInput(), engine->GetSceneMgr());
    camera->SetControlState(dw::CCS_FREE);
    engine->SetMainCamera(camera);
    engine->SetStarSystem(new dw::StarSystem(engine->GetRenderer(), engine->GetPhysicsWorld()));
    engine->Run([camera](float dt) {camera->Update(dt);});
    delete camera; // This needs to be deleted before engine->Shutdown() because deleting the camera
                   // unregisters it from the event listener
    engine->Shutdown();
    return 0;
}
