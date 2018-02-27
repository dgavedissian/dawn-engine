/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "DawnEngine.h"
#include "renderer/MeshBuilder.h"
#include "renderer/Mesh.h"

#include "Ship.h"

#include "net/NetData.h"
#include "net/NetGameMode.h"

using namespace dw;

class ShooterEntityPipeline : public EntityPipeline {
public:
    DW_OBJECT(ShooterEntityPipeline);

    Vector<SharedPtr<Ship>> ship_list_;

    explicit ShooterEntityPipeline(Context* ctx) : EntityPipeline(ctx) {
    }

    ~ShooterEntityPipeline() override = default;

    u32 getEntityMetadata(const Entity& entity) override {
        return 0;
    }

    Entity& createEntityFromMetadata(EntityId entity_id, u32 metadata, NetRole role) override {
        assert(metadata == 0);  // only one entity type supported.
        SharedPtr<Ship> ship = makeShared<Ship>(context(), entity_id, role);
        Entity& entity = *ship->entity();
        entity.component<ShipControls>()->ship = ship;
        ship_list_.emplace_back(std::move(ship));
        return entity;
    }
};

class ShooterGameMode : public NetGameMode {
public:
    DW_OBJECT(ShooterGameMode);

    ShooterGameMode(Context* ctx, ShooterEntityPipeline* entity_pipeline)
        : NetGameMode(ctx), entity_pipeline_(entity_pipeline) {
    }

    // NetGameMode
    void clientOnJoinServer() override {
        log().info("Client: connected to server.");
        subsystem<NetSystem>()->sendSpawnRequest(
            0,
            [this](Entity& entity) {
                log().info("Received spawn response. Triggering callback.");
                camera_controller->follow(&entity);
            },
            /* messaging_proxy */ true);
    }

    void serverOnStart() override {
        // ship = makeShared<Ship>(context());
        // camera_controller->follow(ship->entity());
    }

    void serverOnClientConnected() override {
        log().info("Server: Client connected.");
    }

    void serverOnClientDisconnected() override {
        log().info("Server: Client disconnected.");
    }

    // GameMode
    void onStart() override {
        NetGameMode::onStart();

        subsystem<Universe>()->createStarSystem();

        // Random thing.
        auto rc = subsystem<ResourceCache>();
        auto material = makeShared<Material>(
            context(), makeShared<Program>(context(), rc->get<VertexShader>("base:space/planet.vs"),
                                           rc->get<FragmentShader>("base:space/planet.fs")));
        material->setTexture(rc->get<Texture>("base:space/planet.jpg"));
        material->setUniform("light_direction", Vec3{0.0f, 0.0f, 1.0f});
        material->setUniform("surface_sampler", 0);
        auto renderable =
            MeshBuilder(context()).texcoords(true).normals(true).createSphere(1000.0f);
        renderable->setMaterial(material);
        subsystem<Universe>()
            ->createEntity(Position{4000.0f, 0.0f, 0.0f}, Quat::identity)
            .addComponent<RenderableComponent>(renderable);

        // Create a camera.
        auto& camera = subsystem<Universe>()
                           ->createEntity(Position{0.0f, 0.0f, 50.0f}, Quat::identity)
                           .addComponent<Camera>(0.1f, 100000.0f, 60.0f, 1280.0f / 800.0f);
        camera_controller = makeShared<ShipCameraController>(context(), Vec3{0.0f, 15.0f, 50.0f});
        camera_controller->possess(&camera);
    }

    void onEnd() override {
        NetGameMode::onEnd();
    }

    void update(float dt) override {
        NetGameMode::update(dt);
        for (auto ship : entity_pipeline_->ship_list_) {
            ship->update(dt);
        }
        camera_controller->update(dt);
    }

private:
    ShooterEntityPipeline* entity_pipeline_;
    SharedPtr<ShipCameraController> camera_controller;
};

class Shooter : public App {
public:
    DW_OBJECT(Shooter);

    void init(int argc, char** argv) override {
        auto rc = subsystem<ResourceCache>();
        assert(rc);
        rc->addPath("base", "../media/base");
        rc->addPath("shooter", "../media/shooter");

        auto entity_pipeline = makeUnique<ShooterEntityPipeline>(context());
        auto entity_pipeline_ptr = entity_pipeline.get();
        subsystem<NetSystem>()->setEntityPipeline(std::move(entity_pipeline));
        subsystem<SystemManager>()->addSystem<ShipEngineSystem>();
        subsystem<GameFramework>()->setGameMode(
            makeShared<ShooterGameMode>(context(), entity_pipeline_ptr));
    }

    void update(float dt) override {
    }

    void render() override {
        subsystem<Renderer>()->setViewClear(0, {0.0f, 0.0f, 0.0f, 1.0f});

        // Calculate average FPS.
        float current_fps = 1.0 / engine_->frameTime();
        static const int FPS_HISTORY_COUNT = 100;
        static float fps_history[FPS_HISTORY_COUNT];
        for (int i = 1; i < FPS_HISTORY_COUNT; ++i) {
            fps_history[i - 1] = fps_history[i];
        }
        fps_history[FPS_HISTORY_COUNT - 1] = current_fps;
        float average_fps = 0.0f;
        for (int i = 0; i < FPS_HISTORY_COUNT; ++i) {
            average_fps += fps_history[i] / FPS_HISTORY_COUNT;
        }

        // Update displayed FPS information every 100ms.
        static double accumulated_time = 0.0;
        static float displayed_fps = 60.0f;
        accumulated_time += engine_->frameTime();
        if (accumulated_time > 1.0f / 30.0f) {
            accumulated_time = 0;
            displayed_fps = average_fps;
        }

        // Display FPS information.
        ImGui::SetNextWindowPos({10, 10});
        ImGui::SetNextWindowSize({140, 40});
        if (!ImGui::Begin("FPS", nullptr, {0, 0}, 0.5f,
                          ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings)) {
            ImGui::End();
            return;
        }
        ImGui::Text("FPS:   %.1f", displayed_fps);
        ImGui::Text("Frame: %.4f ms", 1000.0f / displayed_fps);
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
