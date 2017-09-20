/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "DawnEngine.h"
#include "ecs/EntityManager.h"
#include "ecs/SystemManager.h"
#include "renderer/Program.h"
#include "renderer/MeshBuilder.h"
#include "resource/ResourceCache.h"
#include "scene/SceneManager.h"
#include "scene/Transform.h"
#include "renderer/Mesh.h"
#include "ui/Imgui.h"

using namespace dw;

class Sandbox : public App {
public:
    DW_OBJECT(Sandbox);

    Entity* object;
    Entity* camera;

    void init(int argc, char** argv) override {
        auto rc = subsystem<ResourceCache>();
        assert(rc);
        rc->addResourceLocation("../media/base");
        rc->addResourceLocation("../media/sandbox");

        // Create a material.
        auto material = makeShared<Material>(
            context(), makeShared<Program>(context(), rc->get<VertexShader>("ship.vs"),
                                           rc->get<FragmentShader>("ship.fs")));
        material->program()->setUniform("light_direction", Vec3{1.0f, 1.0f, 1.0f}.Normalized());

        auto renderable = rc->get<Mesh>("models/core-large.mesh.xml");
        renderable->setMaterial(material);
        auto sphere = rc->get<Mesh>("models/side-wing.mesh.xml");
        sphere->setMaterial(material);

        // Create entities.Wor
        auto em = subsystem<EntityManager>();
        object = &em->createEntity()
            .addComponent<Transform>(Position{-10.0f, 0.0f, 0.0f}, Quat::identity, subsystem<SceneManager>()->rootNode())
            .addComponent<RenderableComponent>(renderable);
        em->createEntity()
            .addComponent<Transform>(Position{8.0f, 0.0f, 0.0f}, Quat::identity, *object)
            .addComponent<RenderableComponent>(sphere);
        em->createEntity()
            .addComponent<Transform>(Position{-8.0f, 0.0f, 0.0f}, Quat::identity, *object)
            .addComponent<RenderableComponent>(sphere);

        // Create a camera.
        camera = &em->createEntity()
            .addComponent<Transform>(Position{0.0f, 0.0f, 50.0f}, Quat::identity, subsystem<SceneManager>()->rootNode())
            .addComponent<Camera>(0.1f, 1000.0f, 60.0f, 1280.0f / 800.0f);
    }

    void update(float dt) override {
        static float angle = 0.0f;
        angle += dt;
        // camera->component<Transform>()->position.x = sin(angle) * 30.0f;
        object->component<Transform>()->orientation() = Quat::RotateY(angle);
    }

    void render() override {
        subsystem<Renderer>()->setViewClear(0, {0.0f, 0.0f, 0.2f, 1.0f});

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
        if (accumulated_time > 1.0f / 30.0f)
        {
            accumulated_time = 0;
            displayed_fps = average_fps;
        }

        // Display FPS information.
        ImGui::SetNextWindowPos({10, 10});
        ImGui::SetNextWindowSize({140, 40});
        if (!ImGui::Begin("FPS", nullptr, {0, 0}, 0.5f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings))
        {
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

DW_IMPLEMENT_MAIN(Sandbox);
