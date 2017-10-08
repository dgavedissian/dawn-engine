/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "DawnEngine.h"
#include "ecs/EntityManager.h"
#include "ecs/Component.h"
#include "ecs/System.h"
#include "renderer/Program.h"
#include "renderer/MeshBuilder.h"
#include "resource/ResourceCache.h"
#include "scene/CameraController.h"
#include "scene/Transform.h"
#include "scene/Universe.h"
#include "renderer/BillboardSet.h"
#include "renderer/Mesh.h"
#include "ui/Imgui.h"

using namespace dw;

class Planet : public Object {
public:
    DW_OBJECT(Planet)

    Planet(Context* ctx, float radius) : Object{ctx}, planet_{nullptr}, radius_{radius} {
        auto em = subsystem<EntityManager>();
        auto rc = subsystem<ResourceCache>();

        // Set up material.
        auto material = makeShared<Material>(
                context(),
                makeShared<Program>(context(), rc->get<VertexShader>("space/planet.vs"),
                                    rc->get<FragmentShader>("space/planet.fs")));
        material->setTextureUnit(rc->get<Texture>("space/planet.jpg"));
        material->setUniform("light_direction", Vec3{1.0f, 0.0f, 0.0f});
        material->setUniform("surface_sampler", 0);
        auto renderable = MeshBuilder(context()).texcoords(true).normals(true).createSphere(radius);
        renderable->setMaterial(material);

        planet_ = &em->createEntity(Position::origin, Quat::identity)
                .addComponent<RenderableComponent>(renderable);
    }

    Position& position() const {
        return planet_->transform()->position();
    }

    float radius() const {
        return radius_;
    }

private:
    Entity* planet_;
    float radius_;
};

class Sandbox : public App {
public:
    DW_OBJECT(Sandbox);

    SharedPtr<CameraController> camera_controller;
    SharedPtr<Planet> planet_;

    void init(int argc, char** argv) override {
        auto rc = subsystem<ResourceCache>();
        assert(rc);
        rc->addResourceLocation("../media/base");
        rc->addResourceLocation("../media/sandbox");

        const float radius = 1000.0f;
        planet_ = makeShared<Planet>(context(), radius);

        // Create a camera.
        auto& camera = subsystem<EntityManager>()
                           ->createEntity(Position{0.0f, 0.0f, radius * 2}, Quat::identity)
                           .addComponent<Camera>(0.1f, 10000.0f, 60.0f, 1280.0f / 800.0f);
        camera_controller = makeShared<CameraController>(context(), 300.0f);
        camera_controller->possess(&camera);
    }

    void update(float dt) override {
        // Calculate distance to planet and adjust acceleration accordingly.
        auto& a = camera_controller->possessed()->transform()->position();
        auto& b = planet_->position();
        float altitude = a.getRelativeTo(b).Length() - planet_->radius();
        camera_controller->setAcceleration(altitude);

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

DW_IMPLEMENT_MAIN(Sandbox);
