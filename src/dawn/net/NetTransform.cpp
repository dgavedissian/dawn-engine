/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "net/NetData.h"
#include "net/NetTransform.h"

namespace dw {
NetTransformSyncSystem::NetTransformSyncSystem(Context* context) : System(context) {
    supportsComponents<Transform, NetTransform, NetData>();
}

void NetTransformSyncSystem::processEntity(Entity& entity, float dt) {
    NetRole role = entity.component<NetData>()->role();
    Transform& transform = *entity.component<Transform>();
    NetTransformState& net_state = entity.component<NetTransform>()->transform_state;

    if (role == NetRole::Authority) {
        // Copy position/orientation state from transform.
        const Position& current_position = transform.position();
        const Quat& current_orientation = transform.orientation();

        // Calculate velocities.
        net_state.velocity = current_position.getRelativeTo(net_state.position) / dt;
        net_state.angular_velocity = net_state.orientation.Inverted() * current_orientation;
        Vec3 angular_velocity_axis;
        float angular_velocity_speed;
        net_state.angular_velocity.ToAxisAngle(angular_velocity_axis, angular_velocity_speed);
        angular_velocity_speed /= dt;
        net_state.angular_velocity.SetFromAxisAngle(angular_velocity_axis, angular_velocity_speed);

        // Apply new state.
        net_state.position = current_position;
        net_state.orientation = current_orientation;
    } else {
        // Update transform.
        transform.position() = net_state.position + net_state.velocity * dt;
        transform.orientation() = net_state.orientation;
    }
}
}  // namespace dw
