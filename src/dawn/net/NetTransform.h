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
    Quat orientation;
    Quat angular_velocity;

    NetTransformState()
        : position(Position::origin),
          velocity(Vec3::zero),
          orientation(Quat::identity),
          angular_velocity(Quat::identity) {
    }

    bool operator==(const NetTransformState& other) const {
        const float eps = 0.01f;
        return position == other.position && velocity.DistanceSq(other.velocity) < 0.01f &&
               orientation.Dot(other.orientation) > (1.0f - eps) &&
               angular_velocity.Dot(other.angular_velocity) > (1.0f - eps);
    }

    bool operator!=(const NetTransformState& other) const {
        return !(*this == other);
    }
};

// TODO: Compress
namespace stream {
template <> NetTransformState read<NetTransformState>(InputStream& s) {
    NetTransformState output;
    s.read(output.position);
    s.read(output.velocity);
    s.read(output.orientation);
    s.read(output.angular_velocity);
    return output;
}

template <> void write<NetTransformState>(OutputStream& s, const NetTransformState& state) {
    s.write(state.position);
    s.write(state.velocity);
    s.write(state.orientation);
    s.write(state.angular_velocity);
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
