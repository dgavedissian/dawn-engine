/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "DawnEngine.h"
#include "renderer/MeshBuilder.h"
#include "renderer/Mesh.h"

#include "Ship.h"

using namespace dw;

class Shooter : public App {
public:
    DW_OBJECT(Shooter);

    SharedPtr<Ship> ship;

    SharedPtr<ShipCameraController> camera_controller;

    void init(int argc, char** argv) override {
        auto rc = subsystem<ResourceCache>();
        assert(rc);
        rc->addPath("base", "../media/base");
        rc->addPath("shooter", "../media/shooter");

        subsystem<SystemManager>()->addSystem<ShipEngineSystem>();

        ship = makeShared<Ship>(context());

        subsystem<Universe>()->createStarSystem();

        // Random thing.
        auto material = makeShared<Material>(
            context(), makeShared<Program>(context(), rc->get<VertexShader>("base:space/planet.vs"),
                                           rc->get<FragmentShader>("base:space/planet.fs")));
        material->setTexture(rc->get<Texture>("base:space/planet.jpg"));
        material->setUniform("light_direction", Vec3{0.0f, 0.0f, 1.0f});
        material->setUniform("surface_sampler", 0);
        auto renderable =
            MeshBuilder(context()).texcoords(true).normals(true).createSphere(1000.0f);
        renderable->setMaterial(material);
        subsystem<EntityManager>()
            ->createEntity(Position{4000.0f, 0.0f, 0.0f}, Quat::identity)
            .addComponent<RenderableComponent>(renderable);

        // Create a camera.
        auto& camera = subsystem<EntityManager>()
                           ->createEntity(Position{0.0f, 0.0f, 50.0f}, Quat::identity)
                           .addComponent<Camera>(0.1f, 100000.0f, 60.0f, 1280.0f / 800.0f);
        camera_controller = makeShared<ShipCameraController>(context(), Vec3{0.0f, 15.0f, 50.0f});
        camera_controller->possess(&camera);
        camera_controller->follow(ship->entity());
    }

    void update(float dt) override {
        ship->update(dt);
        camera_controller->update(dt);
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
        return "Sandbox";
    }

    String gameVersion() override {
        return "1.0.0";
    }
};

DW_IMPLEMENT_MAIN(Shooter);
