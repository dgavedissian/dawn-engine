/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "DawnEngine.h"
#include "renderer/MeshBuilder.h"
#include "renderer/Mesh.h"
#include "CProjectile.h"
#include "CWeapon.h"
#include "ShooterGameMode.h"

using namespace dw;

class Shooter : public App {
public:
    DW_OBJECT(Shooter);

    void init(int argc, char** argv) override {
        auto rc = module<ResourceCache>();
        auto sm = module<SceneManager>();

        assert(rc);
        rc->addPath("base", "../media/base");
        rc->addPath("shooter", "../media/shooter");

        // Create frame.
        auto* frame = module<Renderer>()->sceneGraph().addFrame(
            module<Renderer>()->sceneGraph().root().newChild());

        // Set up game.
        auto entity_pipeline = makeUnique<ShooterEntityPipeline>(context(), frame);
        auto entity_pipeline_ptr = entity_pipeline.get();
        module<Networking>()->setEntityPipeline(std::move(entity_pipeline));
        sm->addSystem<SShipEngines>();
        sm->addSystem<SProjectile>(
            frame,
            HashMap<int, ProjectileTypeInfo>{
                {0, {100.0f, {6.0f, 15.0f}, rc->get<Texture>("shooter:weapons/projectile1.jpg")}},
                {1, {100.0f, {6.0f, 15.0f}, rc->get<Texture>("shooter:weapons/projectile2.jpg")}}});
        sm->addSystem<SWeapon>();
        module<GameplayModule>()->setGameMode(
            makeShared<ShooterGameMode>(context(), frame, entity_pipeline_ptr));
    }

    void update(float dt) override {
    }

    void render(float) override {
        module<Renderer>()->rhi()->setViewClear(0, {0.0f, 0.0f, 0.0f, 1.0f});

        // Display FPS information.
        ImGui::SetNextWindowPos({10, 10});
        ImGui::SetNextWindowSize({140, 40});
        if (!ImGui::Begin("FPS", nullptr,
                          ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings)) {
            ImGui::End();
            return;
        }
        ImGui::Text("FPS:   %d", engine_->framesPerSecond());
        ImGui::Text("Frame: %.4f ms", engine_->frameTime());
        ImGui::End();
    }

    void shutdown() override {
    }

    String gameName() override {
        return typeName();
    }

    String gameVersion() override {
        return "1.0.0";
    }
};

DW_IMPLEMENT_MAIN(Shooter);
