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
#include "renderer/BillboardSet.h"
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
            {1, 0.0f, {0.0f, 20.0f, 0.0f}, {-20.0f, 0.0f, 0.0f}},
            {2, 0.0f, {0.0f, -20.0f, 0.0f}, {-20.0f, 0.0f, 0.0f}},
            {3, 0.0f, {0.0f, 20.0f, 0.0f}, {20.0f, 0.0f, 0.0f}},
            {4, 0.0f, {0.0f, -20.0f, 0.0f}, {20.0f, 0.0f, 0.0f}},

            {5, 0.0f, {20.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 20.0f}},
            {6, 0.0f, {-20.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 20.0f}},
            {7, 0.0f, {20.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -20.0f}},
            {8, 0.0f, {-20.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -20.0f}},

            {9, 0.0f, {0.0f, 0.0f, 20.0f}, {0.0f, 20.0f, 0.0f}},
            {10, 0.0f, {0.0f, 0.0f, -20.0f}, {0.0f, 20.0f, 0.0f}},
            {11, 0.0f, {0.0f, 0.0f, 20.0f}, {0.0f, -20.0f, 0.0f}},
            {12, 0.0f, {0.0f, 0.0f, -20.0f}, {0.0f, -20.0f, 0.0f}},
        };

        // Get engine information projected onto each rotation axis.
        Vector<Pair<String, Vec3>> rotation_axes = {
            {"yz plane (pitch - x rotation)", Vec3{1.0f, 0.0f, 0.0f}},
            {"xz plane (yaw - y rotation)", Vec3{0.0f, -1.0f, 0.0f}},
            {"xy plane (roll - z rotation)", Vec3{0.0f, 0.0f, -1.0f}}};
        for (int i = 0; i < navigation_engines_.size(); ++i) {
            log().info("For %s", rotation_axes[i].first);
            Plane rotation_plane{rotation_axes[i].second, 0.0f};
            for (auto& engine : engines_) {
                Vec3 proj_force = rotation_plane.Project(engine.force);
                Vec3 proj_position = rotation_plane.Project(engine.position);
                Vec3 cross = proj_force.Cross(proj_position);

                // If the cross product is above the plane, then the direction is clockwise,
                // otherwise anticlockwise.
                float signed_distance = rotation_plane.SignedDistance(cross);
                bool clockwise = signed_distance > 0.0f;

                if (abs(signed_distance) > 0.01f) {
                    log().info("Engine %s %s - Projected %s %s - Direction: %s (%.0f)",
                               engine.force.ToString(), engine.position.ToString(),
                               proj_force.ToString(), proj_position.ToString(),
                               clockwise ? "clockwise" : "anticlockwise", signed_distance);

                    navigation_engines_[i].emplace_back(
                        NavigationEngine{{engine.id, 0.0f, proj_force, proj_position},
                                         &engine,
                                         clockwise ? NavigationEngineDirection::Clockwise
                                                   : NavigationEngineDirection::Anticlockwise});
                }
            }
        }

        Vec3 clockwise;
        Vec3 anticlockwise;
        calculateMaxAngularAcceleration(clockwise, anticlockwise);
        log().info("Max clockwise: %s - anticlockwise: %s", clockwise.ToString(),
                   anticlockwise.ToString());

        // Engine particles.
        particles_ = makeShared<BillboardSet>(context(), engines_.size(), 10.0f);
        subsystem<EntityManager>()
            ->createEntity(Position::origin, Quat::identity, core_)
            .addComponent<RenderableComponent>(particles_);
        particles_->material()->setTextureUnit(rc->get<Texture>("textures/scene-star-glow.png"), 0);
        for (int i = 0; i < engines_.size(); i++) {
        }
    }

    ~Ship() {
    }

    void update(float dt) {
        auto input = subsystem<Input>();

        // Attenuate engine glows.
        for (auto& e : engines_) {
            e.update(dt);
        }

        // Control rotational thrusters.
        float pitch_direction = static_cast<float>(input->isKeyDown(Key::Up)) -
                                static_cast<float>(input->isKeyDown(Key::Down));
        float yaw_direction = static_cast<float>(input->isKeyDown(Key::Right)) -
                              static_cast<float>(input->isKeyDown(Key::Left));
        float roll_direction = static_cast<float>(input->isKeyDown(Key::P)) -
                               static_cast<float>(input->isKeyDown(Key::O));
        fireRotationalThrusters(pitch_direction, yaw_direction, roll_direction);

        // Calculate angular acceleration.
        Vec3 angular_acc =
            calculateAngularAcceleration(pitch_direction, yaw_direction, roll_direction);

        // Get angular velocity relative to the ship.

        // Update engine particles.
        for (int i = 0; i < engines_.size(); i++) {
            float engine_glow_size = 3.0f * engines_[i].visibility;
            particles_->setParticlePosition(
                i, Vec3{core_->transform()->modelMatrix() * Vec4{engines_[i].position, 1.0f}});
            particles_->setParticleSize(i, {engine_glow_size, engine_glow_size});
        }

        // Display stats.
        ImGui::SetNextWindowPos({10, 50});
        ImGui::SetNextWindowSize({300, 60});
        if (!ImGui::Begin("Ship", nullptr, {0, 0}, 0.5f,
                          ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings)) {
            ImGui::End();
            return;
        }
        Vec3 angular_vel = angularVelocity();
        ImGui::Text("Angular Velocity: %.2f %.2f %.2f", angular_vel.x, angular_vel.y,
                    angular_vel.z);
        ImGui::Text("Angular Acceleration: %.2f %.2f %.2f", angular_acc.x, angular_acc.y,
                    angular_acc.z);
        ImGui::End();
    }

    void calculateMaxAngularAcceleration(Vec3& clockwise, Vec3& anticlockwise) const {
        // Pitch - X axis.
        // Yaw - Y axis.
        // Roll - Z axis.
        Vec3 clockwise_torque{0.0f, 0.0f, 0.0f};
        Vec3 anticlockwise_torque{0.0f, 0.0f, 0.0f};
        for (int i = 0; i < navigation_engines_.size(); ++i) {
            for (auto& engine : navigation_engines_[i]) {
                if (engine.direction == NavigationEngineDirection::Clockwise) {
                    clockwise_torque += engine.engine.torque();
                } else {
                    anticlockwise_torque += engine.engine.torque();
                }
            }
        }
        clockwise = rb_->getInvInertiaTensorWorld() * clockwise_torque;
        anticlockwise = rb_->getInvInertiaTensorWorld() * anticlockwise_torque;
    }

    Vec3 calculateAngularAcceleration(float pitch, float yaw, float roll) const {
        Vec3 power{pitch, yaw, roll};
        Vec3 total_torque{0.0f, 0.0f, 0.0f};
        for (int i = 0; i < navigation_engines_.size(); ++i) {
            NavigationEngineDirection direction = power[i] > 0
                                                      ? NavigationEngineDirection::Clockwise
                                                      : NavigationEngineDirection::Anticlockwise;
            for (auto& engine : navigation_engines_[i]) {
                if (engine.direction == direction) {
                    total_torque += engine.engine.torque() * abs(power[i]);
                }
            }
        }
        return rb_->getInvInertiaTensorWorld() * total_torque;
    }

    void fireRotationalThrusters(float pitch, float yaw, float roll) {
        Vec3 power{pitch, yaw, roll};
        Vec3 total_torque{0.0f, 0.0f, 0.0f};
        for (int i = 0; i < navigation_engines_.size(); ++i) {
            NavigationEngineDirection direction = power[i] > 0
                                                      ? NavigationEngineDirection::Clockwise
                                                      : NavigationEngineDirection::Anticlockwise;
            for (auto& engine : navigation_engines_[i]) {
                if (engine.direction == direction) {
                    Vec3 engine_torque = engine.engine.torque() * abs(power[i]);
                    total_torque += engine_torque;
                    if (engine_torque.Length() > 0.01f) {
                        engine.actual_engine->fire();
                    }
                }
            }
        }
        rb_->activate();
        rb_->applyTorque(core_->transform()->orientation() * total_torque);
    }

    Vec3 angularVelocity() const {
        Quat inv_rotation = core_->transform()->orientation();
        inv_rotation.InverseAndNormalize();
        return inv_rotation * Vec3{rb_->getAngularVelocity()};
    }

private:
    Entity* core_;
    btRigidBody* rb_;
    SharedPtr<Material> material_;

    SharedPtr<BillboardSet> particles_;

    struct EngineData {
        int id;
        float visibility;
        Vec3 force;
        Vec3 position;

        void fire() {
            visibility = 1.0f;
        }

        void update(float dt) {
            visibility *= 0.99f;  // damp(visibility, 0.0f, 0.99f, dt);
        }

        Vec3 torque() const {
            // Applying a moment of force (torque) is T = r x F (r is the position, F is the force
            // vector).
            return force.Cross(position);
        }
    };
    Vector<EngineData> engines_;

    enum class NavigationEngineDirection { Clockwise, Anticlockwise };
    struct NavigationEngine {
        EngineData engine;  // TODO: Don't use EngineData here but create a separate structure with
                            // torque()..
        EngineData* actual_engine;
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
