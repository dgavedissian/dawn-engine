/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "DawnEngine.h"
#include "ecs/EntityManager.h"
#include "ecs/SystemManager.h"
#include "ecs/Component.h"
#include "ecs/System.h"
#include "renderer/Program.h"
#include "resource/ResourceCache.h"
#include "scene/CameraController.h"
#include "scene/Transform.h"
#include "scene/Universe.h"
#include "renderer/BillboardSet.h"
#include "renderer/Mesh.h"
#include "ui/Imgui.h"

using namespace dw;

class ShipEngineData {
public:
    ShipEngineData(const Vec3& force, const Vec3& offset);

    void fire();
    void update(float dt);

    float activity() const;
    Vec3 force() const;
    Vec3 offset() const;

private:
    float activity_;
    Vec3 force_;
    Vec3 offset_;
};

ShipEngineData::ShipEngineData(const Vec3 &force, const Vec3 &offset) : activity_(0.0f), force_(force), offset_(offset) {
}

void ShipEngineData::fire() {
    activity_ = 1.0f;
}

void ShipEngineData::update(float dt) {
    activity_ = damp(activity_, 0.0f, 0.99f, dt);
}

float ShipEngineData::activity() const {
    return activity_;
}

Vec3 ShipEngineData::force() const {
    return force_;
}

Vec3 ShipEngineData::offset() const {
    return offset_;
}

class ShipEngineInstance {
public:
    ShipEngineInstance(ShipEngineData* parent, const Vec3& force, const Vec3& offset);

    Vec3 torque() const;
    ShipEngineData* parent() const;

private:
    Vec3 force_;
    Vec3 offset_;
    ShipEngineData* parent_;

};

ShipEngineInstance::ShipEngineInstance(ShipEngineData *parent, const Vec3 &force,
                                       const Vec3 &offset) : force_(force), offset_(offset), parent_(parent) {
}

Vec3 ShipEngineInstance::torque() const {
    // Applying a moment of force (torque) is T = r x F (r is the position, F is the force
    // vector).
    return force_.Cross(offset_);
}

ShipEngineData *ShipEngineInstance::parent() const {
    return parent_;
}

class ShipEngines : public Component, public Object {
public:
    DW_OBJECT(ShipEngines);

    ShipEngines(Context* ctx, const Vector<ShipEngineData>& engine_data);

    void onAddToEntity(Entity* parent) override;

    // Movement engines.

    // Rotational engines.
    void calculateMaxRotationalTorque(Vec3& clockwise, Vec3& anticlockwise) const;
    Vec3 calculateRotationalTorque(float pitch, float yaw, float roll) const;
    Vec3 fireRotationalEngines(float pitch, float yaw, float roll);

private:
    Vector<ShipEngineData> engine_data_;
    SharedPtr<BillboardSet> glow_billboards_;
    SharedPtr<BillboardSet> trail_billboards_;

    // Navigational engines.

    // Rotational engines.
    enum class NavigationEngineDirection { Clockwise, Anticlockwise };
    enum RotationAxis { RotationAxis_X, RotationAxis_Y, RotationAxis_Z };
    struct NavigationEngine {
        NavigationEngine(const ShipEngineInstance& engine, NavigationEngineDirection direction) : engine{engine}, direction{direction} {}
        ShipEngineInstance engine;
        NavigationEngineDirection direction;
    };
    Array<Vector<NavigationEngine>, 3> navigation_engines_;

    friend class ShipEngineSystem;
};

class ShipEngineSystem : public System {
public:
    DW_OBJECT(ShipEngineSystem);

    explicit ShipEngineSystem(Context* ctx) : System(ctx) {
        supportsComponents<Transform, RigidBody, ShipEngines>();
    }

    void processEntity(Entity& entity, float dt) override {
        auto& transform = *entity.component<Transform>();
        auto& rigid_body = *entity.component<RigidBody>();
        auto& ship_engines = *entity.component<ShipEngines>();

        auto& engines = ship_engines.engine_data_;

        // Update particles.
        if (ship_engines.glow_billboards_) {
            for (int i = 0; i < engines.size(); i++) {
                float engine_glow_size = 2.0f * engines[i].activity();
                ship_engines.glow_billboards_->setParticlePosition(
                    i,
                    Vec3{ transform.modelMatrix() * Vec4 { engines[i].offset(), 1.0f } });
                ship_engines.glow_billboards_->setParticleSize(i, { engine_glow_size, engine_glow_size });
                ship_engines.trail_billboards_->setParticlePosition(
                    i,
                    Vec3{ transform.modelMatrix() * Vec4 { engines[i].offset(), 1.0f } });
                ship_engines.trail_billboards_->setParticleSize(i, { engine_glow_size * 0.25f, engine_glow_size * 3.0f });
                ship_engines.trail_billboards_->setParticleDirection(i, Vec3{ transform.modelMatrix() * Vec4 { engines[i].force().Normalized(), 0.0f } });
            }
        }

        // Attenuate engines.
        for (auto& e : engines) {
            e.update(dt);
        }
    }
};

ShipEngines::ShipEngines(Context* ctx, const Vector<ShipEngineData> &engine_data) : Object(ctx), engine_data_(engine_data) {
    // Generate navigation engines by projecting onto each rotation axis.
    Vector<Pair<String, Vec3>> rotation_axes = {
            {"yz plane (pitch - x rotation)", Vec3{1.0f, 0.0f, 0.0f}},
            {"xz plane (yaw - y rotation)", Vec3{0.0f, -1.0f, 0.0f}},
            {"xy plane (roll - z rotation)", Vec3{0.0f, 0.0f, -1.0f}}};
    for (int i = 0; i < navigation_engines_.size(); ++i) {
        log().info("For %s", rotation_axes[i].first);
        Plane rotation_plane{rotation_axes[i].second, 0.0f};
        for (auto& engine : engine_data_) {
            Vec3 proj_force = rotation_plane.Project(engine.force());
            Vec3 proj_position = rotation_plane.Project(engine.offset());
            Vec3 cross = proj_force.Cross(proj_position);

            // If the cross product is above the plane, then the direction is clockwise,
            // otherwise anticlockwise.
            float signed_distance = rotation_plane.SignedDistance(cross);
            bool clockwise = signed_distance > 0.0f;

            if (abs(signed_distance) > 0.01f) {
                log().info("Engine %s %s - Projected %s %s - Direction: %s (%.0f)",
                           engine.force().ToString(), engine.offset().ToString(),
                           proj_force.ToString(), proj_position.ToString(),
                           clockwise ? "clockwise" : "anticlockwise", signed_distance);

                navigation_engines_[i].emplace_back(
                        NavigationEngine{ShipEngineInstance{&engine, proj_force, proj_position},
                                         clockwise ? NavigationEngineDirection::Clockwise
                                                   : NavigationEngineDirection::Anticlockwise});
            }
        }
    }
}

void ShipEngines::onAddToEntity(Entity* parent) {
    // Initialise engine particles.
    auto* renderable_component = parent->component<RenderableComponent>();
    if (renderable_component) {
        glow_billboards_ = makeShared<BillboardSet>(context(), engine_data_.size(), Vec2{ 10.0f, 10.0f });
        glow_billboards_->material()->setTextureUnit(subsystem<ResourceCache>()->get<Texture>("engine/glow.png"), 0);
        renderable_component->node->addChild(glow_billboards_);

        trail_billboards_ = makeShared<BillboardSet>(context(), engine_data_.size(), Vec2{ 10.0f, 10.0f });
        trail_billboards_->material()->setTextureUnit(subsystem<ResourceCache>()->get<Texture>("engine/trail.png"), 0);
        trail_billboards_->setBillboardType(BillboardType::Directional);
        renderable_component->node->addChild(trail_billboards_);
    }
}

void ShipEngines::calculateMaxRotationalTorque(Vec3& clockwise, Vec3& anticlockwise) const {
    // Pitch - X axis.
    // Yaw - Y axis.
    // Roll - Z axis.
    Vec3 clockwise_torque{0.0f, 0.0f, 0.0f};
    Vec3 anticlockwise_torque{0.0f, 0.0f, 0.0f};
    for (int i = 0; i < navigation_engines_.size(); ++i) {
        for (auto& nav_engine : navigation_engines_[i]) {
            if (nav_engine.direction == NavigationEngineDirection::Clockwise) {
                clockwise_torque += nav_engine.engine.torque();
            } else {
                anticlockwise_torque += nav_engine.engine.torque();
            }
        }
    }
    clockwise = clockwise_torque;
    anticlockwise = anticlockwise_torque;
}

Vec3 ShipEngines::calculateRotationalTorque(float pitch, float yaw, float roll) const {
    Vec3 power{pitch, yaw, roll};
    Vec3 total_torque{0.0f, 0.0f, 0.0f};
    for (int i = 0; i < navigation_engines_.size(); ++i) {
        NavigationEngineDirection direction = power[i] > 0
                                                  ? NavigationEngineDirection::Clockwise
                                                  : NavigationEngineDirection::Anticlockwise;
        for (auto& nav_engine : navigation_engines_[i]) {
            if (nav_engine.direction == direction) {
                total_torque += nav_engine.engine.torque() * abs(power[i]);
            }
        }
    }
    return total_torque;
}

Vec3 ShipEngines::fireRotationalEngines(float pitch, float yaw, float roll) {
    Vec3 power{pitch, yaw, roll};
    Vec3 total_torque{0.0f, 0.0f, 0.0f};
    for (int i = 0; i < navigation_engines_.size(); ++i) {
        NavigationEngineDirection direction = power[i] > 0
                                                  ? NavigationEngineDirection::Clockwise
                                                  : NavigationEngineDirection::Anticlockwise;
        for (auto& nav_engine : navigation_engines_[i]) {
            if (nav_engine.direction == direction) {
                Vec3 engine_torque = nav_engine.engine.torque() * abs(power[i]);
                total_torque += engine_torque;
                if (engine_torque.Length() > 0.01f) {
                    nav_engine.engine.parent()->fire();
                }
            }
        }
    }
    return total_torque;
}

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
                                          makeShared<btBoxShape>(btVector3{10.0f, 10.0f, 10.0f}))
                 .addComponent<ShipEngines>(context(), Vector<ShipEngineData>{
            // 4 on right, 4 on left
                     { { 40.0f, 0.0f, 0.0f },{ 5.0f, 2.0f, 10.0f } },
                     { { 40.0f, 0.0f, 0.0f },{ 5.0f, -2.0f, 10.0f } },
                     { { 40.0f, 0.0f, 0.0f },{ 5.0f, 2.0f, -10.0f } },
                     { { 40.0f, 0.0f, 0.0f },{ 5.0f, -2.0f, -10.0f } },
                     { { -40.0f, 0.0f, 0.0f },{ -5.0f, 2.0f, 10.0f } },
                     { { -40.0f, 0.0f, 0.0f },{ -5.0f, -2.0f, 10.0f } },
                     { { -40.0f, 0.0f, 0.0f },{ -5.0f, 2.0f, -10.0f } },
                     { { -40.0f, 0.0f, 0.0f },{ -5.0f, -2.0f, -10.0f } },

                     // 4 on top, 4 on bottom.
                     { { 0.0f, 40.0f, 0.0f },{ 2.0f, 5.0f, 10.0f } },
                     { { 0.0f, 40.0f, 0.0f },{ -2.0f, 5.0f, 10.0f } },
                     { { 0.0f, 40.0f, 0.0f },{ 2.0f, 5.0f, -10.0f } },
                     { { 0.0f, 40.0f, 0.0f },{ -2.0f, 5.0f, -10.0f } },
                     { { 0.0f, -40.0f, 0.0f },{ 2.0f, -5.0f, 10.0f } },
                     { { 0.0f, -40.0f, 0.0f },{ -2.0f, -5.0f, 10.0f } },
                     { { 0.0f, -40.0f, 0.0f },{ 2.0f, -5.0f, -10.0f } },
                     { { 0.0f, -40.0f, 0.0f },{ -2.0f, -5.0f, -10.0f } }
        });
        auto node = core_->component<RenderableComponent>()->node;
        node->addChild(makeShared<RenderableNode>(sphere, Vec3{8.0f, 0.0f, 0.0f}, Quat::identity));
        node->addChild(makeShared<RenderableNode>(sphere, Vec3{-8.0f, 0.0f, 0.0f}, Quat::identity));

        // Get rigid body.
        rb_ = core_->component<RigidBody>()->_rigidBody();

        // Get engine stats.
        Vec3 clockwise;
        Vec3 anticlockwise;
        core_->component<ShipEngines>()->calculateMaxRotationalTorque(clockwise, anticlockwise);
        clockwise = core_->component<RigidBody>()->_rigidBody()->getInvInertiaTensorWorld() * clockwise;
        anticlockwise = core_->component<RigidBody>()->_rigidBody()->getInvInertiaTensorWorld() * anticlockwise;
        log().info("Max clockwise: %s - anticlockwise: %s", clockwise.ToString(),
                   anticlockwise.ToString());
    }

    ~Ship() {
    }

    void update(float dt) {
        auto input = subsystem<Input>();

        auto& engines = *core_->component<ShipEngines>();
        auto& rb = *core_->component<RigidBody>();

        // Control rotational thrusters.
        float pitch_direction = static_cast<float>(input->isKeyDown(Key::Up)) -
                                static_cast<float>(input->isKeyDown(Key::Down));
        float yaw_direction = static_cast<float>(input->isKeyDown(Key::Right)) -
                              static_cast<float>(input->isKeyDown(Key::Left));
        float roll_direction = static_cast<float>(input->isKeyDown(Key::P)) -
                               static_cast<float>(input->isKeyDown(Key::O));
        fireRotationalThrusters(pitch_direction, yaw_direction, roll_direction);

        // Calculate angular acceleration.
        Vec3 angular_acc = rb._rigidBody()->getInvInertiaTensorWorld() *
                engines.calculateRotationalTorque(pitch_direction, yaw_direction, roll_direction);

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

    void fireRotationalThrusters(float pitch, float yaw, float roll) {
        Vec3 total_torque = core_->component<ShipEngines>()->fireRotationalEngines(pitch, yaw, roll);
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
};

class Shooter : public App {
public:
    DW_OBJECT(Shooter);

    SharedPtr<Ship> ship;

    SharedPtr<CameraController> camera_controller;

    void init(int argc, char** argv) override {
        auto rc = subsystem<ResourceCache>();
        assert(rc);
        rc->addResourceLocation("../media/base");
        rc->addResourceLocation("../media/shooter");

        subsystem<SystemManager>()->addSystem<ShipEngineSystem>();

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

DW_IMPLEMENT_MAIN(Shooter);
