/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "DawnEngine.h"
#include "scene/Transform.h"
#include "net/NetData.h"
#include "net/NetTransform.h"
#include "Ship.h"

using namespace dw;

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

    // Follow behind.
    if (followed_->hasComponent<RigidBody>()) {
        Vec3 velocity = followed_->component<RigidBody>()->_rigidBody()->getLinearVelocity();
        float timestep = 1.0f / 60.0f;
        float scale = 3.0f;
        possessed_transform->position() -= velocity * timestep * scale;
    }
}

ShipFlightComputer::ShipFlightComputer(Context* ctx, Ship* ship)
    : Object{ctx},
      ship_{ship},
      target_linear_velocity_{Vec3::zero},
      target_angular_velocity_{Vec3::zero} {
    auto engines = ship->entity()->component<ShipEngines>();
    auto rb = ship->entity()->component<RigidBody>();

    Vec3 pos_force, neg_force;
    engines->calculateMaxMovementForce(pos_force, neg_force);
    ship_acceleration_forwards_ = rb->_rigidBody()->getInvMass() * pos_force;
    ship_acceleration_backwards_ = rb->_rigidBody()->getInvMass() * neg_force;

    Vec3 pos_torque, neg_torque;
    engines->calculateMaxRotationalTorque(pos_torque, neg_torque);
    ship_angular_acceleration_forwards_ = rb->_rigidBody()->getInvInertiaTensorWorld() * pos_torque;
    ship_angular_acceleration_backwards_ =
        rb->_rigidBody()->getInvInertiaTensorWorld() * neg_torque;

    log().info("Max positive: %s - max negative: %s", ship_acceleration_forwards_.ToString(),
               ship_acceleration_backwards_.ToString());
    log().info("Max clockwise: %s - max anticlockwise: %s",
               ship_angular_acceleration_forwards_.ToString(),
               ship_angular_acceleration_backwards_.ToString());
}

void ShipFlightComputer::setTargetLinearVelocity(const Vec3& target_linear_velocity) {
    target_linear_velocity_ = target_linear_velocity;
}

void ShipFlightComputer::setTargetAngularVelocity(const Vec3& target_angular_velocity) {
    target_angular_velocity_ = target_angular_velocity;
}

void ShipFlightComputer::update(float dt) {
    // Define reducer method.
    auto vec_reducer = [](const Vec3& source, const Vec3& target, const Vec3& max_pos_speed,
                          const Vec3& max_neg_speed) -> Vec3 {
        auto reducer = [](float source, float target, float max_pos_speed,
                          float max_neg_speed) -> float {
            float diff = target - source;
            if (diff > 0.0f) {
                // Motion is positive, capped at max_pos_speed.
                return min(diff, max_pos_speed);
            } else if (diff < 0.0f) {
                // Motion is negative, capped at max_neg_speed.
                return -min(-diff, -max_neg_speed);
            }
            return 0.0f;
        };
        return Vec3{reducer(source.x, target.x, max_pos_speed.x, max_neg_speed.x),
                    reducer(source.y, target.y, max_pos_speed.y, max_neg_speed.y),
                    reducer(source.z, target.z, max_pos_speed.z, max_neg_speed.z)};
    };

    // Calculate engine power to apply.
    float timestep = 1.0f / 60.0f;
    Vec3 movement_acceleration = vec_reducer(ship_->localVelocity(), target_linear_velocity_,
                                             ship_acceleration_forwards_ * timestep,
                                             ship_acceleration_backwards_ * timestep) /
                                 timestep;
    Vec3 rotational_acceleration = vec_reducer(ship_->angularVelocity(), target_angular_velocity_,
                                               ship_angular_acceleration_forwards_ * timestep,
                                               ship_angular_acceleration_backwards_ * timestep) /
                                   timestep;

    // Apply.
    Vec3 movement_power{ShipEngines::convertToPower(
        movement_acceleration, ship_acceleration_forwards_, ship_acceleration_backwards_)};
    if (movement_acceleration.Length() > 0.01f) {
        ship_->fireMovementThrusters(movement_power);
    }
    Vec3 angular_power{ShipEngines::convertToPower(rotational_acceleration,
                                                   ship_angular_acceleration_forwards_,
                                                   ship_angular_acceleration_backwards_)};
    if (rotational_acceleration.Length() > 0.01f) {
        ship_->fireRotationalThrusters(angular_power);
    }

    // Display stats.
    ImGui::SetNextWindowPos({10, 200});
    ImGui::SetNextWindowSize({300, 120});
    if (!ImGui::Begin("Ship Flight", nullptr, {0, 0}, 0.5f,
                      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                          ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings)) {
        ImGui::End();
        return;
    }
    Vec3 movement_vel = ship_->localVelocity();
    Vec3 angular_vel = ship_->angularVelocity();
    ImGui::Text("Movement velocity: %.2f %.2f %.2f", movement_vel.x, movement_vel.y,
                movement_vel.z);
    ImGui::Text("Movement acceleration: %.2f %.2f %.2f", movement_acceleration.x,
                movement_acceleration.y, movement_acceleration.z);
    ImGui::Text("Movement power: %.2f %.2f %.2f", movement_power.x, movement_power.y,
                movement_power.z);
    ImGui::Text("Angular velocity: %.2f %.2f %.2f", angular_vel.x, angular_vel.y, angular_vel.z);
    ImGui::Text("Angular acceleration: %.2f %.2f %.2f", rotational_acceleration.x,
                rotational_acceleration.y, rotational_acceleration.z);
    ImGui::Text("Angular power: %.2f %.2f %.2f", angular_power.x, angular_power.y, angular_power.z);
    ImGui::End();
}

Ship::Ship(Context* ctx)
    : Ship(ctx, ctx->module<SceneManager>()->reserveEntityId(), NetRole::Authority) {
}

Ship::Ship(Context* ctx, EntityId reserved_entity_id, NetRole role) : Object(ctx), rb_(nullptr) {
    auto rc = module<ResourceCache>();
    assert(rc);

    material_ = makeShared<Material>(
        context(), makeShared<Program>(context(), rc->get<VertexShader>("shooter:ship.vs"),
                                       rc->get<FragmentShader>("shooter:ship.fs")));
    material_->program()->setUniform("light_direction", Vec3{1.0f, 1.0f, 1.0f}.Normalized());

    auto renderable = rc->get<Mesh>("shooter:models/core-large.mesh.xml");
    renderable->setMaterial(material_);
    auto sphere = rc->get<Mesh>("shooter:models/side-wing.mesh.xml");
    sphere->setMaterial(material_);

    // Create ship entity.
    auto sm = module<SceneManager>();
    ship_entity_ = &sm->createEntity(Hash("Ship"), reserved_entity_id)
                        .addComponent<Transform>(Position{0.0f, 0.0f, 0.0f}, Quat::identity)
                        .addComponent<RenderableComponent>(renderable)
                        .addComponent<ShipEngines>(
                            context(),
                            Vector<ShipEngineData>{// 4 on back.
                                                   {{0.0f, 0.0f, -400.0f}, {5.0f, 0.0f, 15.0f}},
                                                   {{0.0f, 0.0f, -400.0f}, {3.0f, 0.0f, 15.0f}},
                                                   {{0.0f, 0.0f, -400.0f}, {-3.0f, 0.0f, 15.0f}},
                                                   {{0.0f, 0.0f, -400.0f}, {-5.0f, 0.0f, 15.0f}},
                                                   // 2 on front.
                                                   {{0.0f, 0.0f, 400.0f}, {5.0f, 0.0f, -12.0f}},
                                                   {{0.0f, 0.0f, 400.0f}, {-5.0f, 0.0f, -12.0f}},
                                                   // 2 on each side
                                                   {{-400.0f, 0.0f, 0.0f}, {6.0f, 0.0f, 8.0f}},
                                                   {{-400.0f, 0.0f, 0.0f}, {6.0f, 0.0f, -8.0f}},
                                                   {{400.0f, 0.0f, 0.0f}, {-6.0f, 0.0f, 8.0f}},
                                                   {{400.0f, 0.0f, 0.0f}, {-6.0f, 0.0f, -8.0f}},
                                                   // 2 above and below
                                                   {{0.0f, -400.0f, 0.0f}, {0.0f, 5.0f, 8.0f}},
                                                   {{0.0f, -400.0f, 0.0f}, {0.0f, 5.0f, -8.0f}},
                                                   {{0.0f, 400.0f, 0.0f}, {0.0f, -5.0f, 8.0f}},
                                                   {{0.0f, 400.0f, 0.0f}, {0.0f, -5.0f, -8.0f}}},
                            Vector<ShipEngineData>{// 4 on right, 4 on left
                                                   {{-35.0f, 0.0f, 0.0f}, {5.0f, 2.0f, 10.0f}},
                                                   {{-35.0f, 0.0f, 0.0f}, {5.0f, -2.0f, 10.0f}},
                                                   {{-35.0f, 0.0f, 0.0f}, {5.0f, 2.0f, -10.0f}},
                                                   {{-35.0f, 0.0f, 0.0f}, {5.0f, -2.0f, -10.0f}},
                                                   {{35.0f, 0.0f, 0.0f}, {-5.0f, 2.0f, 10.0f}},
                                                   {{35.0f, 0.0f, 0.0f}, {-5.0f, -2.0f, 10.0f}},
                                                   {{35.0f, 0.0f, 0.0f}, {-5.0f, 2.0f, -10.0f}},
                                                   {{35.0f, 0.0f, 0.0f}, {-5.0f, -2.0f, -10.0f}},

                                                   // 4 on top, 4 on bottom.
                                                   {{0.0f, -35.0f, 0.0f}, {2.0f, 5.0f, 10.0f}},
                                                   {{0.0f, -35.0f, 0.0f}, {-2.0f, 5.0f, 10.0f}},
                                                   {{0.0f, -35.0f, 0.0f}, {2.0f, 5.0f, -10.0f}},
                                                   {{0.0f, -35.0f, 0.0f}, {-2.0f, 5.0f, -10.0f}},
                                                   {{0.0f, 35.0f, 0.0f}, {2.0f, -5.0f, 10.0f}},
                                                   {{0.0f, 35.0f, 0.0f}, {-2.0f, -5.0f, 10.0f}},
                                                   {{0.0f, 35.0f, 0.0f}, {2.0f, -5.0f, -10.0f}},
                                                   {{0.0f, 35.0f, 0.0f}, {-2.0f, -5.0f, -10.0f}}});
    auto node = ship_entity_->component<RenderableComponent>()->node;
    node->addChild(makeShared<RenderableNode>(sphere, Vec3{8.0f, 0.0f, 0.0f}, Quat::identity));
    node->addChild(makeShared<RenderableNode>(sphere, Vec3{-8.0f, 0.0f, 0.0f}, Quat::identity,
                                              Vec3{-1.0f, 1.0f, 1.0f}));

    // Networking.
    ship_entity_->addComponent<NetTransform>();
    ship_entity_->addComponent<ShipControls>();
    ship_entity_->addComponent<NetData>(
        RepLayout::build<NetTransform, ShipEngines, ShipControls>());

    // Initialise server-side details.
    if (role >= NetRole::Authority) {
        ship_entity_->addComponent<RigidBody>(
            module<SceneManager>()->physicsScene(), 10.0f,
            makeShared<btBoxShape>(btVector3{10.0f, 10.0f, 10.0f}));
        rb_ = ship_entity_->component<RigidBody>()->_rigidBody();

        // Initialise flight computer.
        flight_computer_ = makeShared<ShipFlightComputer>(context(), this);
    }
}

void Ship::update(float dt) {
    auto input = module<Input>();

    // auto& engines = *ship_entity_->component<ShipEngines>();
    auto& controls = *ship_entity_->component<ShipControls>();
    auto& net_data = *ship_entity_->component<NetData>();

    // Update based on net role.
    if (net_data.role() >= NetRole::Authority) {
        //=============================
        // Handle authoritative server.
        //=============================

        // Apply controls to flight computer.
        flight_computer_->setTargetLinearVelocity(controls.target_linear_velocity);
        flight_computer_->setTargetAngularVelocity(controls.target_angular_velocity);

        // Update flight computer.
        if (flight_computer_) {
            flight_computer_->update(dt);
        }

        // Calculate angular acceleration.
        /*Vec3 angular_acc = Vec3(
                rb_->getInvInertiaTensorWorld() *
                engines.calculateRotationalTorque({pitch_direction, yaw_direction,
           roll_direction}));*/
        Vec3 angular_vel = angularVelocity();

        // Display stats.
        ImGui::SetNextWindowPos({10, 50});
        ImGui::SetNextWindowSize({300, 60});
        if (!ImGui::Begin("Ship", nullptr, {0, 0}, 0.5f,
                          ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings)) {
            ImGui::End();
            return;
        }
        ImGui::Text("Angular Velocity: %.2f %.2f %.2f", angular_vel.x, angular_vel.y,
                    angular_vel.z);
        // ImGui::Text("Angular Acceleration: %.2f %.2f %.2f", angular_acc.x, angular_acc.y,
        //            angular_acc.z);
        ImGui::End();
    } else if (net_data.role() == NetRole::AuthoritativeProxy) {
        //=============================
        // Handle controlling client.
        //=============================

        // Control movement thrusters.
        float x_movement = static_cast<float>(input->isKeyDown(Key::D)) -
                           static_cast<float>(input->isKeyDown(Key::A));
        float y_movement = static_cast<float>(input->isKeyDown(Key::R)) -
                           static_cast<float>(input->isKeyDown(Key::F));
        float z_movement = static_cast<float>(input->isKeyDown(Key::S)) -
                           static_cast<float>(input->isKeyDown(Key::W));
        Vec3 target_linear_velocity = Vec3{x_movement, y_movement, z_movement} * 100.0f;
        if (!controls.target_linear_velocity.BitEquals(target_linear_velocity)) {
            controls.setLinearVelocity(target_linear_velocity);
            controls.target_linear_velocity = target_linear_velocity;
        }

        // Control rotational thrusters.
        float pitch_direction = static_cast<float>(input->isKeyDown(Key::Up)) -
                                static_cast<float>(input->isKeyDown(Key::Down));
        float yaw_direction = static_cast<float>(input->isKeyDown(Key::Left)) -
                              static_cast<float>(input->isKeyDown(Key::Right));
        float roll_direction = static_cast<float>(input->isKeyDown(Key::Q)) -
                               static_cast<float>(input->isKeyDown(Key::E));
        Vec3 target_angular_velocity = Vec3{pitch_direction, yaw_direction, roll_direction} * 1.2f;
        if (!controls.target_angular_velocity.BitEquals(target_angular_velocity)) {
            controls.setAngularVelocity(target_angular_velocity);
            controls.target_angular_velocity = target_angular_velocity;
        }
    }
}

void Ship::fireMovementThrusters(const Vec3& power) {
    Vec3 total_force = ship_entity_->component<ShipEngines>()->fireMovementEngines(power);
    rb_->activate();
    rb_->applyCentralForce(ship_entity_->transform()->orientation() * total_force);
}

void Ship::fireRotationalThrusters(const Vec3& power) {
    Vec3 total_torque = ship_entity_->component<ShipEngines>()->fireRotationalEngines(power);
    rb_->activate();
    rb_->applyTorque(ship_entity_->transform()->orientation() * total_torque);
}

Vec3 Ship::angularVelocity() const {
    Quat inv_rotation = ship_entity_->transform()->orientation();
    inv_rotation.InverseAndNormalize();
    return inv_rotation * Vec3{rb_->getAngularVelocity()};
}

Vec3 Ship::localVelocity() const {
    Quat inv_rotation = ship_entity_->transform()->orientation();
    inv_rotation.InverseAndNormalize();
    return inv_rotation * Vec3{rb_->getLinearVelocity()};
}

Entity* Ship::entity() const {
    return ship_entity_;
}
