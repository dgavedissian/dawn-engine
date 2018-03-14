/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "scene/Component.h"
#include "scene/System.h"
#include "math/Defs.h"
#include "scene/Position.h"
#include "io/InputStream.h"
#include "io/OutputStream.h"

namespace dw {
struct NetTransformState {
    Position position;
    Vec3 velocity;
    Vec3 acceleration;
    Quat orientation;
    Vec3 angular_velocity;
    Vec3 angular_acceleration;

    NetTransformState()
        : position(Position::origin),
          velocity(Vec3::zero),
          acceleration(Vec3::zero),
          orientation(Quat::identity),
          angular_velocity(Vec3::zero),
          angular_acceleration(Vec3::zero) {
    }

    bool operator==(const NetTransformState& other) const {
        const float eps = 0.01f;
        return position == other.position &&
               velocity.DistanceSq(other.velocity) < 0.01f &&
               acceleration.DistanceSq(other.acceleration) < 0.01f &&
               orientation.Dot(other.orientation) > (1.0f - eps) &&
               angular_velocity.DistanceSq(other.angular_velocity) < 0.01f &&
               angular_acceleration.DistanceSq(other.angular_acceleration) < 0.01f;
    }

    bool operator!=(const NetTransformState& other) const {
        return !(*this == other);
    }
};

// TODO: Compress
namespace stream {
template <> inline NetTransformState read<NetTransformState>(InputStream& s) {
    NetTransformState output;
    s.read(output.position);
    s.read(output.velocity);
    s.read(output.acceleration);
    s.read(output.orientation);
    s.read(output.angular_velocity);
    s.read(output.angular_acceleration);
    return output;
}

template <> inline  void write<NetTransformState>(OutputStream& s, const NetTransformState& state) {
    s.write(state.position);
    s.write(state.velocity);
    s.write(state.acceleration);
    s.write(state.orientation);
    s.write(state.angular_velocity);
    s.write(state.angular_acceleration);
}
}  // namespace stream

class NetTransform : public Component {
public:
    NetTransformState transform_state;

	static RepLayout repLayout()
	{
		return {{RepProperty::bind<NetTransform>(&NetTransform::transform_state)}, {}};
	}
};

class NetTransformSyncSystem : public System {
public:
    DW_OBJECT(NetTransformSyncSystem);

    NetTransformSyncSystem(Context* context);
    ~NetTransformSyncSystem() = default;

    void processEntity(Entity& entity, float dt) override;
};
}  // namespace dw
