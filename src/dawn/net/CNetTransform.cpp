/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "net/CNetData.h"
#include "net/CNetTransform.h"
#include "scene/PhysicsScene.h"

namespace dw {
void SNetTransformSync::process(float dt) {
    for (auto e : entityView()) {
        auto entity = Entity{scene_mgr_, e};

        NetRole role = entity.component<CNetData>()->role();
        CSceneNode& transform = *entity.component<CSceneNode>();
        NetTransformState& net_state = entity.component<CNetTransform>()->transform_state;
        CRigidBody* rigid_body = entity.component<CRigidBody>();

        if (role >= NetRole::Authority) {
            // If this entity has a rigid body, calculate velocity/angular velocity.
            float inv_dt = 1.0f / dt;
            if (rigid_body) {
                // Physics state velocities assume a dt of 1.0f / 60.0f
                float inv_physics_timestep = 60.0f;
                net_state.velocity =
                    rigid_body->_rigidBody()->getLinearVelocity() * inv_physics_timestep;
                net_state.acceleration = rigid_body->_rigidBody()->getInvMass() *
                                         rigid_body->_rigidBody()->getTotalForce() *
                                         inv_physics_timestep;
                net_state.angular_velocity =
                    rigid_body->_rigidBody()->getAngularVelocity() * inv_physics_timestep;
                net_state.angular_acceleration =
                    rigid_body->_rigidBody()->getInvInertiaTensorWorld() *
                    rigid_body->_rigidBody()->getTotalTorque() * inv_physics_timestep;
            } else {
                net_state.velocity =
                    (transform.node->transform().position - net_state.position) / inv_dt;
                net_state.angular_velocity = Vec3::zero;
                net_state.angular_acceleration = Vec3::zero;
            }

            // Apply new state.
            net_state.position = transform.node->transform().position;
            net_state.orientation = transform.node->transform().orientation;
        } else {
            /*
    // Update transform and integrate velocities.
    transform.position() = net_state.position + net_state.velocity * dt + 0.5f *
    net_state.acceleration
    * dt * dt;
    // TODO: Combine the next 6 lines into one line once operator+ is implemented properly.
    Quat new_orientation = net_state.orientation;
    Vec3 integrated_angular_velocity = net_state.angular_velocity + net_state.angular_acceleration *
    dt; Quat integrated_delta_rotation = Quat(integrated_angular_velocity.x,
    integrated_angular_velocity.y, integrated_angular_velocity.z, 0.0f) * net_state.orientation;
    new_orientation.x += dt * 0.5f * integrated_delta_rotation.x; new_orientation.y += dt * 0.5f *
    integrated_delta_rotation.y; new_orientation.z += dt * 0.5f * integrated_delta_rotation.z;
    new_orientation.w += dt * 0.5f * integrated_delta_rotation.w;
    new_orientation.Normalize();
    transform.orientation() = new_orientation;
            */
            auto& xform = transform.node->transform();
            xform.position = net_state.position;
            xform.orientation = net_state.orientation;
        }
    }
}
}  // namespace dw
