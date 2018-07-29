/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "DawnEngine.h"
#include "scene/CTransform.h"
#include "net/NetData.h"
#include "net/CNetTransform.h"
#include "Ship.h"
#include "ShipFlightComputer.h"

using namespace dw;

Ship::Ship(Context* ctx, Frame* frame)
    : Ship(ctx, frame, ctx->module<SceneManager>()->reserveEntityId(), NetRole::Authority) {
}

Ship::Ship(Context* ctx, Frame* frame, EntityId reserved_entity_id, NetRole role)
    : Object(ctx), rb_(nullptr), ship_entity_(nullptr) {
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
                        .addComponent<CTransform>(frame->newChild(), renderable)
                        .addComponent<CShipEngines>(
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
    auto node = ship_entity_->component<CTransform>()->node;
    node->newChild(Vec3{8.0f, 0.0f, 0.0f}, Quat::identity)->data.renderable = sphere;
    node->newChild(Vec3{-8.0f, 0.0f, 0.0f}, Quat::identity, Vec3{-1.0f, 1.0f, 1.0f})
        ->data.renderable = sphere;

    // Networking.
    ship_entity_->addComponent<CNetTransform>();
    ship_entity_->addComponent<CShipControls>();
    if (role != NetRole::None) {
        ship_entity_->addComponent<NetData>(
            RepLayout::build<CNetTransform, CShipEngines, CShipControls>());
    }

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

    // auto& engines = *ship_entity_->component<CShipEngines>();
    auto& controls = *ship_entity_->component<CShipControls>();
    auto net_data = ship_entity_->component<NetData>();

    if (!net_data || net_data->role() == NetRole::AuthoritativeProxy) {
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
            if (net_data) {
                controls.setLinearVelocity(target_linear_velocity);
            }
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
            if (net_data) {
                controls.setAngularVelocity(target_angular_velocity);
            }
            controls.target_angular_velocity = target_angular_velocity;
        }
    }

    // Update based on net role.
    if (!net_data || net_data->role() >= NetRole::Authority) {
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
        if (!ImGui::Begin("Ship", nullptr,
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
    }
}

void Ship::fireMovementThrusters(const Vec3& power) {
    Vec3 total_force = ship_entity_->component<CShipEngines>()->fireMovementEngines(power);
    rb_->activate();
    rb_->applyCentralForce(ship_entity_->transform()->orientation * total_force);
}

void Ship::fireRotationalThrusters(const Vec3& power) {
    Vec3 total_torque = ship_entity_->component<CShipEngines>()->fireRotationalEngines(power);
    rb_->activate();
    rb_->applyTorque(ship_entity_->transform()->orientation * total_torque);
}

Vec3 Ship::angularVelocity() const {
    Quat inv_rotation = ship_entity_->transform()->orientation;
    inv_rotation.InverseAndNormalize();
    return inv_rotation * Vec3{rb_->getAngularVelocity()};
}

Vec3 Ship::localVelocity() const {
    Quat inv_rotation = ship_entity_->transform()->orientation;
    inv_rotation.InverseAndNormalize();
    return inv_rotation * Vec3{rb_->getLinearVelocity()};
}

Entity* Ship::entity() const {
    return ship_entity_;
}
