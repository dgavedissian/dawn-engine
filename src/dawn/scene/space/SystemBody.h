/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "renderer/Node.h"
#include "scene/space/Orbit.h"

namespace dw {
// Base class for any bodies inside a planetary system
class DW_API SystemBody : public Object {
public:
    DW_OBJECT(SystemBody);

    SystemBody(Context* context, float radius, SystemNode& system_node);
    ~SystemBody() override = default;

    /// Add a satellite.
    SystemBody& addSatellite(UniquePtr<SystemBody> satellite, UniquePtr<Orbit> orbit);

    /// Update this body.
    virtual void update(float dt, const SystemPosition& camera_position);

    /// Pre-render this body.
    virtual void preRender();

    // Accessors
    float radius() const;
    SystemNode& getSystemNode() const;
    const Orbit& getOrbit() const;
    const SystemBody& getSatellite(uint index) const;
    const Vector<SharedPtr<SystemBody>>& getAllSatellites() const;

    // Internal: Calculate the position of this object and all satellites
    // at a specific time
    virtual void updatePosition(double time);

protected:
    float radius_;
    SystemNode& system_node_;
    UniquePtr<Orbit> orbit_;

    SystemBody* parent_;
    Vector<SharedPtr<SystemBody>> satellites_;
};
}  // namespace dw
