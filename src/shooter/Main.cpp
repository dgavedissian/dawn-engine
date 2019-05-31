/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Core.h"
#include "resource/ResourceCache.h"
#include "renderer/Mesh.h"
#include "renderer/Renderer.h"
#include "CProjectile.h"
#include "CWeapon.h"
#include "ShooterGameMode.h"
#include "core/GameSession.h"
#include "renderer/SceneGraph.h"
#include "renderer/Texture.h"

using namespace dw;

class ShooterGameSession : public GameSession {
public:
    DW_OBJECT(ShooterGameSession);

    ShooterGameSession(Context* ctx, const GameSessionInfo& gsi) : GameSession(ctx, gsi) {
        auto rc = module<ResourceCache>();

        if (!net_instance_ || net_instance_->netMode() == NetMode::Client) {
            module<Input>()->registerEventSystem(event_system_.get());
        }

        // Create frame.
        auto* frame = scene_graph_->addFrame(scene_graph_->root().newChild());

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
                {0,
                 {100.0f,
                  {6.0f, 15.0f},
                  rc->get<Texture>("shooter:weapons/projectile1.jpg").value()}},
                {1,
                 {100.0f,
                  {6.0f, 15.0f},
                  rc->get<Texture>("shooter:weapons/projectile2.jpg").value()}}});
        scene_manager_->addSystem<SWeapon>();

        // Start the game.
        setGameMode(makeShared<ShooterGameMode>(context(), this, frame, entity_pipeline));
    }

    ~ShooterGameSession() override {
        if (!net_instance_ || net_instance_->netMode() == NetMode::Client) {
            module<Input>()->unregisterEventSystem(event_system_.get());
        }
    }

    void update(float dt) override {
        GameSession::update(dt);

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
        ImGui::Text("FPS:   %f", 1.0f / dt);
        ImGui::Text("Frame: %.4f ms", dt);
        ImGui::End();
    }
};

class Shooter : public App {
public:
    DW_OBJECT(Shooter);

    Shooter() : App(typeNameStatic(), "1.0.0") {
    }

    void init(const CommandLine& cmdline) override {
        // Add multiple game sessions
        if (cmdline.flags.find("-two_sessions") != cmdline.flags.end()) {
            const u16 port = 40000;

            GameSessionInfo server_session;
            server_session.start_info = GameSessionInfo::CreateNetGame{
                "127.0.0.1", port, 32, "TestScene", NetTransport::InProcess};
            server_session.headless = true;
            GameSessionInfo client_session;
            client_session.start_info =
                GameSessionInfo::JoinNetGame{"127.0.0.1", port, NetTransport::InProcess};

            engine_->addSession(makeUnique<ShooterGameSession>(context(), server_session));
            engine_->addSession(makeUnique<ShooterGameSession>(context(), client_session));
        } else {
            // Add game session.
            GameSessionInfo gsi;
            auto port_arg = cmdline.arguments.find("-p");
            u16 port = port_arg != cmdline.arguments.end() ? std::stoi(port_arg->second) : 40000;
            if (cmdline.flags.find("-host") != cmdline.flags.end()) {
                gsi.start_info = GameSessionInfo::CreateNetGame{"127.0.0.1", port, 32, "TestScene"};
            } else if (cmdline.arguments.find("-join") != cmdline.arguments.end()) {
                gsi.start_info = GameSessionInfo::JoinNetGame{cmdline.arguments.at("-join"), port};
            }
            gsi.headless = cmdline.flags.find("-headless") != cmdline.flags.end();
            engine_->addSession(makeUnique<ShooterGameSession>(context(), gsi));
        }
    }

    void shutdown() override {
    }
};

DW_IMPLEMENT_MAIN(Shooter);
