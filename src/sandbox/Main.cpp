/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "DawnEngine.h"
#include "ecs/EntityManager.h"
#include "renderer/Program.h"
#include "resource/ResourceCache.h"
#include "scene/CameraController.h"
#include "scene/Transform.h"
#include "scene/Universe.h"
#include "renderer/Mesh.h"
#include "ui/Imgui.h"

using namespace dw;

class Ship : public Object {
public:
    DW_OBJECT(Ship);

    Ship(Context* ctx) : Object(ctx) {
        auto rc = subsystem<ResourceCache>();
        assert(rc);

        material_ = makeShared<Material>(
            context(), makeShared<Program>(context(), rc->get<VertexShader>("ship.vs"),
                                           rc->get<FragmentShader>("ship.fs")));
        material_->program()->setUniform("light_direction", Vec3{1.0f, 1.0f, 1.0f}.Normalized());

        auto renderable = rc->get<Mesh>("models/core-large.mesh.xml");
        renderable->setMaterial(material_);
        auto sphere = rc->get<Mesh>("models/side-wing.mesh.xml");
        sphere->setMaterial(material_);

        // Create entities.
        auto em = subsystem<EntityManager>();
        core_ =
            &em->createEntity(Position{-10.0f, 0.0f, 0.0f}, Quat::identity)
                 .addComponent<RenderableComponent>(renderable)
                 .addComponent<RigidBody>(subsystem<PhysicsSystem>(), 10.0f,
                                          makeShared<btBoxShape>(btVector3{10.0f, 10.0f, 10.0f}));
        em->createEntity(Position{8.0f, 0.0f, 0.0f}, Quat::identity, core_)
            .addComponent<RenderableComponent>(sphere);
        em->createEntity(Position{-8.0f, 0.0f, 0.0f}, Quat::identity, core_)
            .addComponent<RenderableComponent>(sphere);

        // Get rigid body.
        rb_ = core_->component<RigidBody>()->_rigidBody();

        // rb_->applyCentralImpulse(btVector3{ 100.0f, 0.0f, 0.0f });

        // Engines.
        engines_ = {
            {1, {0.0f, 10.0f, 0.0f}, {-30.0f, 0.0f, 0.0f}},   // clockwise
            {2, {0.0f, -10.0f, 0.0f}, {-30.0f, 0.0f, 0.0f}},  // anticlockwise
            {3, {0.0f, 10.0f, 0.0f}, {30.0f, 0.0f, 0.0f}},    // anticlockwise
            {4, {0.0f, -10.0f, 0.0f}, {30.0f, 0.0f, 0.0f}},   // clockwise
        };

        // Get engine information projected onto each rotation axis.
        Vector<Pair<String, Vec3>> rotation_axes = {
            {"yz plane (pitch - x rotation)", Vec3{1.0f, 0.0f, 0.0f}},
            {"xz plane (yaw - y rotation)", Vec3{0.0f, 1.0f, 0.0f}},
            {"xy plane (roll - z rotation)", Vec3{0.0f, 0.0f, 1.0f}}};
        for (int i = 0; i < navigation_engines_.size(); ++i) {
            log().info("For %s", rotation_axes[i].first);
            Plane rotation_plane{rotation_axes[i].second, 0.0f};
            for (auto& engine : engines_) {
                Vec3 proj_direction = rotation_plane.Project(engine.direction);
                Vec3 proj_position = rotation_plane.Project(engine.position);
                float dot = proj_direction.Dot(proj_position);
                Vec3 cross = proj_direction.Cross(proj_position);

                // If the cross product is above the plane, then the direction is clockwise,
                // otherwise anticlockwise.
                float signed_distance = rotation_plane.SignedDistance(cross);
                bool clockwise = signed_distance > 0.0f;

                log().info("Engine %s %s - Projected %s %s - Dot: %.0f - Direction: %s (%.0f)",
                           engine.direction.ToString(), engine.position.ToString(),
                           proj_direction.ToString(), proj_position.ToString(), dot,
                           clockwise ? "clockwise" : "anticlockwise", signed_distance);

                if (abs(signed_distance) > 0.01f) {
                    navigation_engines_[i].emplace_back(
                        NavigationEngine{engine,
                                         clockwise ? NavigationEngineDirection::Clockwise
                                                   : NavigationEngineDirection::Anticlockwise});
                }
            }
        }
    }

    ~Ship() {
    }

    void update(float dt) {
        auto input = subsystem<Input>();

        Vec3 angular_acc{ 0.0f, 0.0f, 0.0f };

        int roll_direction = static_cast<int>(input->isKeyDown(Key::Right)) -
                             static_cast<int>(input->isKeyDown(Key::Left));
        if (roll_direction != 0) {
            Vec3 total_torque{ 0.0f, 0.0f, 0.0f };
            NavigationEngineDirection direction = roll_direction == 1
                                                      ? NavigationEngineDirection::Clockwise
                                                      : NavigationEngineDirection::Anticlockwise;
            for (auto& engine : navigation_engines_[RotationAxis_Z]) {
                if (engine.direction == direction) {
                    // Applying a moment of force (torque) is T = r x F (r is the position, F is
                    // the force vector).
                    total_torque += engine.engine.position.Cross(engine.engine.direction);
                }
            }
            rb_->activate();
            rb_->applyTorque(total_torque);

            angular_acc = rb_->getInvInertiaTensorWorld() * total_torque;
        }

        // Display stats.
        ImGui::SetNextWindowPos({ 10, 50 });
        ImGui::SetNextWindowSize({ 300, 40 });
        if (!ImGui::Begin("Ship", nullptr, { 0, 0 }, 0.5f,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings)) {
            ImGui::End();
            return;
        }
        auto angular_vel = Vec3{ rb_->getAngularVelocity() };
        ImGui::Text("Angular Velocity: %.2f %.2f %.2f", angular_vel.x, angular_vel.y, angular_vel.z);
        ImGui::Text("Angular Acceleration: %.2f %.2f %.2f", angular_acc.x, angular_acc.y, angular_acc.z);
        ImGui::End();
    }

private:
    Entity* core_;
    btRigidBody* rb_;
    SharedPtr<Material> material_;

    struct Engine {
        int id;
        Vec3 direction;
        Vec3 position;
    };
    Vector<Engine> engines_;

    enum class NavigationEngineDirection { Clockwise, Anticlockwise };
    struct NavigationEngine {
        Engine engine;
        NavigationEngineDirection direction;
    };
    enum RotationAxis { RotationAxis_X, RotationAxis_Y, RotationAxis_Z };
    Array<Vector<NavigationEngine>, 3> navigation_engines_;
};

class Sandbox : public App {
public:
    DW_OBJECT(Sandbox);

    SharedPtr<Ship> ship;

    SharedPtr<CameraController> camera_controller;

    void init(int argc, char** argv) override {
        auto rc = subsystem<ResourceCache>();
        assert(rc);
        rc->addResourceLocation("../media/base");
        rc->addResourceLocation("../media/sandbox");

        ship = makeShared<Ship>(context());

        // Create a camera.
        auto& camera = subsystem<EntityManager>()
                           ->createEntity(Position{0.0f, 0.0f, 50.0f}, Quat::identity)
                           .addComponent<Camera>(0.1f, 1000.0f, 60.0f, 1280.0f / 800.0f);
        camera_controller = makeShared<CameraController>(context(), 300.0f);
        camera_controller->possess(&camera);
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

DW_IMPLEMENT_MAIN(Sandbox);
