/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Core.h"
#include "Renderer.h"
#include "Resource.h"
#include "Scene.h"
#include "UI.h"
#include "scene/space/PlanetLod.h"
#include "scene/space/StarSystem.h"

using namespace dw;

class SandboxSession : public GameSession {
public:
    DW_OBJECT(SandboxSession);

    UniquePtr<StarSystem> star_system_;

    Frame* frame_;
    SharedPtr<CameraController> camera_controller;

    SandboxSession(Context* ctx, const GameSessionInfo& gsi) : GameSession(ctx, gsi) {
        module<Input>()->registerEventSystem(event_system_.get());

        // Star system.
        star_system_ = makeUnique<StarSystem>(context(), scene_graph_->root());
        auto& star =
            star_system_->addStar(StarDesc{1000.0f, SpectralClass::G}, star_system_->root(),
                                  makeUnique<CircularOrbit>(0.0f, 1.0f));
        PlanetDesc planet_desc;
        planet_desc.radius = 100.0f;
        planet_desc.surface_texture = "base:space/planet.jpg";
        auto& planet = star_system_->addPlanet(planet_desc, star,
                                               makeUnique<CircularOrbit>(4000.0f, 10000.0f));

        PlanetDesc moon_desc;
        moon_desc.radius = 20.0f;
        moon_desc.surface_texture = "base:space/moon.jpg";
        star_system_->addPlanet(moon_desc, planet, makeUnique<CircularOrbit>(300.0f, 40.0f));

        // Calculate positions of star system objects.
        star_system_->updatePosition(0.0);

        // Create frame of reference for camera.
        auto* frame_root = scene_graph_->root().newChild();
        frame_root->position = planet.getSystemNode().position;
        frame_root->position.z += planet.radius() * 2.0f;
        frame_ = scene_graph_->addFrame(frame_root);

        // Create a camera.
        auto& camera = scene_manager_->createEntity(0, Vec3::zero, Quat::identity, *frame_);
        camera.addComponent<CCamera>(0.1f, 10000.0f, 60.0f, 1280.0f / 800.0f);
        camera_controller = makeShared<CameraController>(context(), event_system_.get(), 300.0f);
        camera_controller->possess(&camera);
    }

    ~SandboxSession() override {
        module<Input>()->unregisterEventSystem(event_system_.get());
    }

    void update(float dt) override {
        GameSession::update(dt);

        auto absolute_camera_position =
            frame_->position() + camera_controller->possessed()->transform()->position;

        // Find nearest system body.
        SystemBody* nearest_system_body = nullptr;
        float nearest_system_body_distance = M_INFINITY;
        for (auto* system_body : star_system_->getSystemBodies()) {
            auto& p = system_body->getSystemNode().position;
            float distance = absolute_camera_position.getRelativeTo(p).Length();
            if (distance < nearest_system_body_distance) {
                nearest_system_body_distance = distance;
                nearest_system_body = system_body;
            }
        }

        // Calculate distance to planet and adjust acceleration accordingly.
        if (nearest_system_body) {
            float altitude = nearest_system_body_distance - nearest_system_body->radius();
            camera_controller->setAcceleration(altitude);

            ImGui::Begin("Object Information", nullptr,
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
            ImGui::SetNextWindowPos({10, 50});
            ImGui::SetNextWindowSize({140, 40});
            ImGui::Text("Nearest object: %s", nearest_system_body->typeName().c_str());
            ImGui::Text("Altitude: %f", altitude);
            ImGui::End();
        }

        camera_controller->update(dt);
        // planet_->update(dt);
    }

    void render(float dt, float interpolation) override {
        GameSession::render(dt, interpolation);

        module<Renderer>()->rhi()->setViewClear(0, {0.0f, 0.0f, 0.1f, 0.2f});

        // Calculate average FPS.
        float current_fps = 1.0f / dt;
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
        accumulated_time += dt;
        if (accumulated_time > 1.0f / 30.0f) {
            accumulated_time = 0;
            displayed_fps = average_fps;
        }

        // Display FPS information.
        ImGui::SetNextWindowPos({10, 10});
        ImGui::SetNextWindowSize({140, 40});
        if (!ImGui::Begin("FPS", nullptr,
                          ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings)) {
            ImGui::End();
            return;
        }
        ImGui::Text("FPS:   %.1f", displayed_fps);
        ImGui::Text("Frame: %.4f ms", 1000.0f / displayed_fps);
        ImGui::End();
    }
};

class Sandbox : public App {
public:
    DW_OBJECT(Sandbox);

    Sandbox() : App("Sandbox", DW_VERSION_STR) {
    }

    void init(const CommandLine& cmdline) override {
        auto rc = module<ResourceCache>();
        assert(rc);
        rc->addPath("base", "../media/base");
        rc->addPath("sandbox", "../media/sandbox");

        engine_->addSession(makeUnique<SandboxSession>(context(), GameSessionInfo{}));
    }

    void shutdown() override {
    }
};

DW_IMPLEMENT_MAIN(Sandbox);
