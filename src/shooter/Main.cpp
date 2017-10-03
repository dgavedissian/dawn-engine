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
#include "renderer/MeshBuilder.h"
#include "resource/ResourceCache.h"
#include "scene/CameraController.h"
#include "scene/Transform.h"
#include "scene/Universe.h"
#include "renderer/BillboardSet.h"
#include "renderer/Mesh.h"
#include "ui/Imgui.h"

using namespace dw;

class ShipCameraController : public Object {
public:
    DW_OBJECT(ShipCameraController);

    explicit ShipCameraController(Context* ctx, const Vec3& offset);
    ~ShipCameraController() = default;

    void follow(Entity* ship);
    void possess(Entity* camera);

    void update(float dt);

private:
    Entity* possessed_;
    Entity* followed_;

    Vec3 offset_;
};

ShipCameraController::ShipCameraController(Context* ctx, const Vec3& offset)
    : Object{ctx}, possessed_{nullptr}, followed_{nullptr}, offset_{offset} {
}

void ShipCameraController::follow(Entity* ship) {
    followed_ = ship;
}

void ShipCameraController::possess(Entity* camera) {
    possessed_ = camera;
}

void ShipCameraController::update(float dt) {
    if (!possessed_ || !followed_) {
        return;
    }
    Transform* possessed_transform = possessed_->transform();
    Transform* followed_transform = followed_->transform();
    if (!possessed_transform || !followed_transform) {
        return;
    }

    possessed_transform->orientation() = followed_transform->orientation();
    possessed_transform->position() =
        followed_transform->position() + followed_transform->orientation() * offset_;
}

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

ShipEngineData::ShipEngineData(const Vec3& force, const Vec3& offset)
    : activity_(0.0f), force_(force), offset_(offset) {
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
    ShipEngineInstance(ShipEngineData* parent, const Vec3& force, const Vec3& offset,
                       bool forwards);

    Vec3 force() const;
    Vec3 offset() const;
    Vec3 torque() const;
    ShipEngineData* parent() const;
    bool isForwards() const;

private:
    Vec3 force_;
    Vec3 offset_;
    ShipEngineData* parent_;
    bool forwards_;
};

ShipEngineInstance::ShipEngineInstance(ShipEngineData* parent, const Vec3& force,
                                       const Vec3& offset, bool forwards)
    : force_(force), offset_(offset), parent_(parent), forwards_(forwards) {
}

Vec3 ShipEngineInstance::force() const {
    return force_;
}

Vec3 ShipEngineInstance::offset() const {
    return offset_;
}

Vec3 ShipEngineInstance::torque() const {
    // Applying a moment of force (torque) is T = r x F (r is the position, F is the force
    // vector).
    return force_.Cross(offset_);
}

ShipEngineData* ShipEngineInstance::parent() const {
    return parent_;
}

bool ShipEngineInstance::isForwards() const {
    return forwards_;
}

class ShipEngines : public Component, public Object {
public:
    DW_OBJECT(ShipEngines);

    ShipEngines(Context* ctx, const Vector<ShipEngineData>& movement_engines,
                const Vector<ShipEngineData>& nav_engines);

    void onAddToEntity(Entity* parent) override;

    // Movement engines.
    Vec3 fireMovementEngines(const Vec3& direction);

    // Rotational engines.
    void calculateMaxRotationalTorque(Vec3& clockwise, Vec3& anticlockwise) const;
    Vec3 calculateRotationalTorque(float pitch, float yaw, float roll) const;
    Vec3 fireRotationalEngines(float pitch, float yaw, float roll);

private:
    Vector<ShipEngineData> engine_data_;
    Vector<ShipEngineData> nav_engine_data_;
    SharedPtr<BillboardSet> glow_billboards_;
    SharedPtr<BillboardSet> trail_billboards_;

    // Navigational engines. [0] == x, [1] == y, [2] == z
    Array<Vector<ShipEngineInstance>, 3> movement_engines_;

    // Rotational engines.
    Array<Vector<ShipEngineInstance>, 3> navigation_engines_;

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
        auto& nav_engines = ship_engines.nav_engine_data_;

        // Update particles.
        if (ship_engines.glow_billboards_) {
            for (int i = 0; i < engines.size(); i++) {
                int particle = i;
                float engine_glow_size = 4.0f * engines[i].activity();
                ship_engines.glow_billboards_->setParticlePosition(
                    particle, Vec3{transform.modelMatrix(Position::origin) *
                                   Vec4{engines[i].offset(), 1.0f}});
                ship_engines.glow_billboards_->setParticleSize(
                    particle, {engine_glow_size, engine_glow_size});
                ship_engines.trail_billboards_->setParticlePosition(
                    particle, Vec3{transform.modelMatrix(Position::origin) *
                                   Vec4{engines[i].offset(), 1.0f}});
                ship_engines.trail_billboards_->setParticleSize(
                    particle, {engine_glow_size * 0.5f, engine_glow_size * 6.0f});
                ship_engines.trail_billboards_->setParticleDirection(
                    particle, Vec3{transform.modelMatrix(Position::origin) *
                                   Vec4{-engines[i].force().Normalized(), 0.0f}});
            }
            for (int i = 0; i < nav_engines.size(); i++) {
                int particle = i + engines.size();
                float engine_glow_size = 2.0f * nav_engines[i].activity();
                ship_engines.glow_billboards_->setParticlePosition(
                    particle, Vec3{transform.modelMatrix(Position::origin) *
                                   Vec4{nav_engines[i].offset(), 1.0f}});
                ship_engines.glow_billboards_->setParticleSize(
                    particle, {engine_glow_size, engine_glow_size});
                ship_engines.trail_billboards_->setParticlePosition(
                    particle, Vec3{transform.modelMatrix(Position::origin) *
                                   Vec4{nav_engines[i].offset(), 1.0f}});
                ship_engines.trail_billboards_->setParticleSize(
                    particle, {engine_glow_size * 0.25f, engine_glow_size * 3.0f});
                ship_engines.trail_billboards_->setParticleDirection(
                    particle, Vec3{transform.modelMatrix(Position::origin) *
                                   Vec4{nav_engines[i].force().Normalized(), 0.0f}});
            }
        }

        // Attenuate engines.
        for (auto& e : engines) {
            e.update(dt);
        }
        for (auto& e : nav_engines) {
            e.update(dt);
        }
    }
};

ShipEngines::ShipEngines(Context* ctx, const Vector<ShipEngineData>& movement_engines,
                         const Vector<ShipEngineData>& nav_engines)
    : Object(ctx), engine_data_(movement_engines), nav_engine_data_(nav_engines) {
    // Generate movement engines.
    Vector<Vec3> movement_axes = {
        {1.0f, 0.0f, 0.0f},  // right.
        {0.0f, 1.0f, 0.0f},  // up.
        {0.0f, 0.0f, -1.0f}  // forward.
    };
    for (uint i = 0; i < movement_engines_.size(); ++i) {
        for (auto& engine : engine_data_) {
            Vec3 proj_force = engine.force().ProjectToNorm(movement_axes[i]);
            bool forwards = proj_force.AngleBetween(movement_axes[i]) < math::pi * 0.5f;
            movement_engines_[i].emplace_back(
                ShipEngineInstance(&engine, proj_force, Vec3::zero, forwards));
        }
    }

    // Generate navigation engines by projecting onto each rotation axis.
    Vector<Pair<String, Vec3>> rotation_axes = {
        {"yz plane (pitch - x rotation)", Vec3{1.0f, 0.0f, 0.0f}},
        {"xz plane (yaw - y rotation)", Vec3{0.0f, -1.0f, 0.0f}},
        {"xy plane (roll - z rotation)", Vec3{0.0f, 0.0f, -1.0f}}};
    for (uint i = 0; i < navigation_engines_.size(); ++i) {
        log().info("For %s", rotation_axes[i].first);
        Plane rotation_plane{rotation_axes[i].second, 0.0f};
        for (auto& engine : nav_engine_data_) {
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
                    ShipEngineInstance{&engine, proj_force, proj_position, clockwise});
            }
        }
    }
}

void ShipEngines::onAddToEntity(Entity* parent) {
    // Initialise engine particles.
    auto* renderable_component = parent->component<RenderableComponent>();
    if (renderable_component) {
        size_t total_engines = engine_data_.size() + nav_engine_data_.size();

        glow_billboards_ = makeShared<BillboardSet>(context(), total_engines, Vec2{10.0f, 10.0f});
        glow_billboards_->material()->setTextureUnit(
            subsystem<ResourceCache>()->get<Texture>("engine/glow.png"), 0);
        renderable_component->node->addChild(glow_billboards_);

        trail_billboards_ = makeShared<BillboardSet>(context(), total_engines, Vec2{10.0f, 10.0f});
        trail_billboards_->material()->setTextureUnit(
            subsystem<ResourceCache>()->get<Texture>("engine/trail.png"), 0);
        trail_billboards_->setBillboardType(BillboardType::Directional);
        renderable_component->node->addChild(trail_billboards_);
    }
}

Vec3 ShipEngines::fireMovementEngines(const Vec3& direction) {
    Vec3 total_force{0.0f, 0.0f, 0.0f};
    for (int i = 0; i < movement_engines_.size(); ++i) {
        bool forwards = direction[i] > 0.0f;
        for (auto& engine : movement_engines_[i]) {
            if (engine.isForwards() == forwards) {
                Vec3 engine_force = engine.force() * abs(direction[i]);
                total_force += engine_force;
                if (engine_force.Length() > 0.01f) {
                    engine.parent()->fire();
                }
            }
        }
    }
    return total_force;
}

void ShipEngines::calculateMaxRotationalTorque(Vec3& clockwise, Vec3& anticlockwise) const {
    // Pitch - X axis.
    // Yaw - Y axis.
    // Roll - Z axis.
    Vec3 clockwise_torque{0.0f, 0.0f, 0.0f};
    Vec3 anticlockwise_torque{0.0f, 0.0f, 0.0f};
    for (int i = 0; i < navigation_engines_.size(); ++i) {
        for (auto& nav_engine : navigation_engines_[i]) {
            if (nav_engine.isForwards()) {
                clockwise_torque += nav_engine.torque();
            } else {
                anticlockwise_torque += nav_engine.torque();
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
        bool forwards = power[i] > 0;
        for (auto& nav_engine : navigation_engines_[i]) {
            if (nav_engine.isForwards() == forwards) {
                total_torque += nav_engine.torque() * abs(power[i]);
            }
        }
    }
    return total_torque;
}

Vec3 ShipEngines::fireRotationalEngines(float pitch, float yaw, float roll) {
    Vec3 power{pitch, yaw, roll};
    Vec3 total_torque{0.0f, 0.0f, 0.0f};
    for (int i = 0; i < navigation_engines_.size(); ++i) {
        bool forwards = power[i] > 0;
        for (auto& nav_engine : navigation_engines_[i]) {
            if (nav_engine.isForwards() == forwards) {
                Vec3 engine_torque = nav_engine.torque() * abs(power[i]);
                total_torque += engine_torque;
                if (engine_torque.Length() > 0.01f) {
                    nav_engine.parent()->fire();
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
                 .addComponent<ShipEngines>(
                     context(),
                     Vector<ShipEngineData>{
                         // 4 on back.
                         {{0.0f, 0.0f, -150.0f}, {5.0f, 0.0f, 15.0f}},
                         {{0.0f, 0.0f, -150.0f}, {3.0f, 0.0f, 15.0f}},
                         {{0.0f, 0.0f, -150.0f}, {-3.0f, 0.0f, 15.0f}},
                         {{0.0f, 0.0f, -150.0f}, {-5.0f, 0.0f, 15.0f}},
                         // 2 on front.
                         {{0.0f, 0.0f, 150.0f}, {5.0f, 0.0f, -12.0f}},
                         {{0.0f, 0.0f, 150.0f}, {-5.0f, 0.0f, -12.0f}},
                     },
                     Vector<ShipEngineData>{// 4 on right, 4 on left
                                            {{30.0f, 0.0f, 0.0f}, {5.0f, 2.0f, 10.0f}},
                                            {{30.0f, 0.0f, 0.0f}, {5.0f, -2.0f, 10.0f}},
                                            {{30.0f, 0.0f, 0.0f}, {5.0f, 2.0f, -10.0f}},
                                            {{30.0f, 0.0f, 0.0f}, {5.0f, -2.0f, -10.0f}},
                                            {{-30.0f, 0.0f, 0.0f}, {-5.0f, 2.0f, 10.0f}},
                                            {{-30.0f, 0.0f, 0.0f}, {-5.0f, -2.0f, 10.0f}},
                                            {{-30.0f, 0.0f, 0.0f}, {-5.0f, 2.0f, -10.0f}},
                                            {{-30.0f, 0.0f, 0.0f}, {-5.0f, -2.0f, -10.0f}},

                                            // 4 on top, 4 on bottom.
                                            {{0.0f, 30.0f, 0.0f}, {2.0f, 5.0f, 10.0f}},
                                            {{0.0f, 30.0f, 0.0f}, {-2.0f, 5.0f, 10.0f}},
                                            {{0.0f, 30.0f, 0.0f}, {2.0f, 5.0f, -10.0f}},
                                            {{0.0f, 30.0f, 0.0f}, {-2.0f, 5.0f, -10.0f}},
                                            {{0.0f, -30.0f, 0.0f}, {2.0f, -5.0f, 10.0f}},
                                            {{0.0f, -30.0f, 0.0f}, {-2.0f, -5.0f, 10.0f}},
                                            {{0.0f, -30.0f, 0.0f}, {2.0f, -5.0f, -10.0f}},
                                            {{0.0f, -30.0f, 0.0f}, {-2.0f, -5.0f, -10.0f}}});
        auto node = core_->component<RenderableComponent>()->node;
        node->addChild(makeShared<RenderableNode>(sphere, Vec3{8.0f, 0.0f, 0.0f}, Quat::identity));
        node->addChild(makeShared<RenderableNode>(sphere, Vec3{-8.0f, 0.0f, 0.0f}, Quat::identity));

        // Get rigid body.
        rb_ = core_->component<RigidBody>()->_rigidBody();

        // Get engine stats.
        Vec3 clockwise;
        Vec3 anticlockwise;
        core_->component<ShipEngines>()->calculateMaxRotationalTorque(clockwise, anticlockwise);
        clockwise =
            core_->component<RigidBody>()->_rigidBody()->getInvInertiaTensorWorld() * clockwise;
        anticlockwise =
            core_->component<RigidBody>()->_rigidBody()->getInvInertiaTensorWorld() * anticlockwise;
        log().info("Max clockwise: %s - anticlockwise: %s", clockwise.ToString(),
                   anticlockwise.ToString());
    }

    ~Ship() {
    }

    void update(float dt) {
        auto input = subsystem<Input>();

        auto& engines = *core_->component<ShipEngines>();
        auto& rb = *core_->component<RigidBody>();

        // Control movement thrusters.
        float movement_direction = static_cast<float>(input->isKeyDown(Key::W)) -
                                   static_cast<float>(input->isKeyDown(Key::S));
        fireMovementThrusters({0.0f, 0.0f, movement_direction});

        // Control rotational thrusters.
        float pitch_direction = static_cast<float>(input->isKeyDown(Key::Up)) -
                                static_cast<float>(input->isKeyDown(Key::Down));
        float yaw_direction = static_cast<float>(input->isKeyDown(Key::Right)) -
                              static_cast<float>(input->isKeyDown(Key::Left));
        float roll_direction = static_cast<float>(input->isKeyDown(Key::E)) -
                               static_cast<float>(input->isKeyDown(Key::Q));
        fireRotationalThrusters(pitch_direction, yaw_direction, roll_direction);

        // Calculate angular acceleration.
        Vec3 angular_acc =
            rb._rigidBody()->getInvInertiaTensorWorld() *
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

    void fireMovementThrusters(const Vec3& direction) {
        Vec3 total_force = core_->component<ShipEngines>()->fireMovementEngines(direction);
        rb_->activate();
        rb_->applyCentralForce(core_->transform()->orientation() * total_force);
    }

    void fireRotationalThrusters(float pitch, float yaw, float roll) {
        Vec3 total_torque =
            core_->component<ShipEngines>()->fireRotationalEngines(pitch, yaw, roll);
        rb_->activate();
        rb_->applyTorque(core_->transform()->orientation() * total_torque);
    }

    Vec3 angularVelocity() const {
        Quat inv_rotation = core_->transform()->orientation();
        inv_rotation.InverseAndNormalize();
        return inv_rotation * Vec3{rb_->getAngularVelocity()};
    }

    Entity* entity() const {
        return core_;
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

    SharedPtr<ShipCameraController> camera_controller;

    void init(int argc, char** argv) override {
        auto rc = subsystem<ResourceCache>();
        assert(rc);
        rc->addResourceLocation("../media/base");
        rc->addResourceLocation("../media/shooter");

        subsystem<SystemManager>()->addSystem<ShipEngineSystem>();

        ship = makeShared<Ship>(context());

        subsystem<Universe>()->createStarSystem();

        // Random thing.
        auto material = makeShared<Material>(
            context(),
            makeShared<Program>(context(), rc->get<VertexShader>("shaders/cube_solid.vs"),
                                rc->get<FragmentShader>("shaders/cube_solid.fs")));
        auto renderable = MeshBuilder(context()).normals(true).createSphere(10.0f);
        renderable->setMaterial(material);
        material->program()->setUniform("light_direction", Vec3{1.0f, 1.0f, 1.0f}.Normalized());
        subsystem<EntityManager>()
            ->createEntity(Position{0.0f, 0.0f, 0.0f}, Quat::identity)
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
