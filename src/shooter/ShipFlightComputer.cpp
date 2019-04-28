/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Core.h"
#include "scene/CSceneNode.h"
#include "net/CNetData.h"
#include "net/CNetTransform.h"
#include "ShipFlightComputer.h"
#include "Ship.h"

using namespace dw;

ShipFlightComputer::ShipFlightComputer(Ship* ship)
    : ship_{ship}, target_linear_velocity{Vec3::zero}, target_angular_velocity{Vec3::zero} {
    auto engines = ship->entity()->component<CShipEngines>();
    auto rb = ship->entity()->component<CRigidBody>();

    Vec3 pos_force, neg_force;
    engines->calculateMaxMovementForce(pos_force, neg_force);
    ship_acceleration_forwards_ = rb->_rigidBody()->getInvMass() * pos_force;
    ship_acceleration_backwards_ = rb->_rigidBody()->getInvMass() * neg_force;

    Vec3 pos_torque, neg_torque;
    engines->calculateMaxRotationalTorque(pos_torque, neg_torque);
    ship_angular_acceleration_forwards_ = rb->_rigidBody()->getInvInertiaTensorWorld() * pos_torque;
    ship_angular_acceleration_backwards_ =
        rb->_rigidBody()->getInvInertiaTensorWorld() * neg_torque;

    /*
    log().info("Max positive: %s - max negative: %s", ship_acceleration_forwards_.ToString(),
               ship_acceleration_backwards_.ToString());
    log().info("Max clockwise: %s - max anticlockwise: %s",
               ship_angular_acceleration_forwards_.ToString(),
               ship_angular_acceleration_backwards_.ToString());
    */
}

void ShipFlightComputerSystem::process(SceneManager* scene_mgr, float dt) {
    entityView(scene_mgr).each([](auto entity, auto& fc) {
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
        Vec3 movement_acceleration =
            vec_reducer(fc.ship_->localVelocity(), fc.target_linear_velocity,
                        fc.ship_acceleration_forwards_ * timestep,
                        fc.ship_acceleration_backwards_ * timestep) /
            timestep;
        Vec3 rotational_acceleration =
            vec_reducer(fc.ship_->angularVelocity(), fc.target_angular_velocity,
                        fc.ship_angular_acceleration_forwards_ * timestep,
                        fc.ship_angular_acceleration_backwards_ * timestep) /
            timestep;

        // Apply.
        Vec3 movement_power{CShipEngines::convertToPower(movement_acceleration,
                                                         fc.ship_acceleration_forwards_,
                                                         fc.ship_acceleration_backwards_)};
        if (movement_acceleration.Length() > 0.01f) {
            fc.ship_->fireMovementThrusters(movement_power);
        }
        Vec3 angular_power{CShipEngines::convertToPower(rotational_acceleration,
                                                        fc.ship_angular_acceleration_forwards_,
                                                        fc.ship_angular_acceleration_backwards_)};
        if (rotational_acceleration.Length() > 0.01f) {
            fc.ship_->fireRotationalThrusters(angular_power);
        }

        // Display stats.
        /*
        ImGui::SetNextWindowPos({10, 200});
        ImGui::SetNextWindowSize({300, 120});
        if (!ImGui::Begin("Ship Flight", nullptr,
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
        ImGui::Text("Angular velocity: %.2f %.2f %.2f", angular_vel.x, angular_vel.y,
        angular_vel.z); ImGui::Text("Angular acceleration: %.2f %.2f %.2f",
        rotational_acceleration.x, rotational_acceleration.y, rotational_acceleration.z);
        ImGui::Text("Angular power: %.2f %.2f %.2f", angular_power.x, angular_power.y,
        angular_power.z); ImGui::End();
         */
    });
}
