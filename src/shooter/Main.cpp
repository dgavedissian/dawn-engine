/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "DawnEngine.h"
#include "renderer/Mesh.h"
#include "CProjectile.h"
#include "CWeapon.h"
#include "ShooterGameMode.h"
#include "gameplay/GameSession.h"

using namespace dw;

class ShooterGameSession : public GameSession {
public:
    DW_OBJECT(ShooterGameSession);

    ShooterGameSession(Context* ctx, const GameSessionInfo& gsi) : GameSession(ctx, gsi) {
        auto rc = module<ResourceCache>();

        // Create frame.
        auto* frame = module<Renderer>()->sceneGraph().addFrame(
            module<Renderer>()->sceneGraph().root().newChild());

        // Set up game.
        auto entity_pipeline = makeShared<ShooterEntityPipeline>(context(), scene_manager_.get(),
                                                                 net_instance_.get(), frame);
        if (net_instance_) {
            net_instance_->setEntityPipeline(entity_pipeline);
        }
        scene_manager_->addSystem<SShipEngines>();
        scene_manager_->addSystem<SProjectile>(
            scene_manager_.get(), net_instance_.get(), frame,
            HashMap<int, ProjectileTypeInfo>{
                {0, {100.0f, {6.0f, 15.0f}, rc->get<Texture>("shooter:weapons/projectile1.jpg")}},
                {1, {100.0f, {6.0f, 15.0f}, rc->get<Texture>("shooter:weapons/projectile2.jpg")}}});
        scene_manager_->addSystem<SWeapon>();

        // Start the game.
        setGameMode(makeShared<ShooterGameMode>(context(), scene_manager_.get(),
                                                net_instance_.get(), frame, entity_pipeline));
    }

    ~ShooterGameSession() override {
    }

    void update(float dt) override {
        GameSession::update(dt);
    }
};

class Shooter : public App {
public:
    DW_OBJECT(Shooter);

    void init(const CommandLine& cmdline) override {
        auto rc = module<ResourceCache>();

        assert(rc);
        rc->addPath("base", "../media/base");
        rc->addPath("shooter", "../media/shooter");

        // Add game session.
        GameSessionInfo gsi;
        auto port_arg = cmdline.arguments.find("-p");
        u16 port = port_arg != cmdline.arguments.end() ? std::stoi(port_arg->second) : 40000;
        if (cmdline.flags.find("-host") != cmdline.flags.end()) {
            gsi.start_info = GameSessionInfo::CreateGame{"127.0.0.1", port, 32, "TestScene"};
        } else if (cmdline.arguments.find("-join") != cmdline.arguments.end()) {
            gsi.start_info = GameSessionInfo::JoinGame{cmdline.arguments.at("-join"), port};
        }
        module<GameplayModule>()->addSession(makeUnique<ShooterGameSession>(context(), gsi));
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
