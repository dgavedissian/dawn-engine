/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "DawnEngine.h"
#include "renderer/MeshBuilder.h"
#include "renderer/Mesh.h"
#include "CProjectile.h"
#include "ShooterGameMode.h"

using namespace dw;

class Shooter : public App {
public:
    DW_OBJECT(Shooter);

    void init(int argc, char** argv) override {
        auto rc = module<ResourceCache>();
        assert(rc);
        rc->addPath("base", "../media/base");
        rc->addPath("shooter", "../media/shooter");

        auto entity_pipeline = makeUnique<ShooterEntityPipeline>(context());
        auto entity_pipeline_ptr = entity_pipeline.get();
        module<Networking>()->setEntityPipeline(std::move(entity_pipeline));
        module<SceneManager>()->addSystem<SShipEngines>();
        module<SceneManager>()->addSystem<SProjectile>();
        module<GameplayModule>()->setGameMode(
            makeShared<ShooterGameMode>(context(), entity_pipeline_ptr));
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
