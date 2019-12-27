/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "renderer/CustomRenderable.h"
#include "renderer/Node.h"

namespace dw {
struct PlanetDesc;

constexpr auto kRingDetailMaxLevel = 14;

class PlanetRings : public Object {
public:
    DW_OBJECT(PlanetRings);

    PlanetRings(Context* ctx, const PlanetDesc& desc, SystemNode& system_node);
    ~PlanetRings() override = default;

    // Propagate updates to the detail hierarchy
    void update(const Vec3& camera_position);  // Camera position must be in planet space
    void updatePositions(const Vec3& planet_position, const Vec3& sun_direction);

    // Get the level of detail distance boundary for a particular level
    float getLodDistance(uint level) const;
    Colour getColour(const Vec2& position) const;  // texture colour component
    float getDensity(const Vec2& position) const;  // texture alpha component

    // Accessors
    float getThickness() const;

private:
    const PlanetDesc& desc_;

    float mMinRadius;
    float mMaxRadius;
    std::array<float, kRingDetailMaxLevel> lod_distance_table_;

    SystemNode& ring_system_node_;
    SharedPtr<CustomRenderable> ring_renderable_;
    SharedPtr<Material> ring_material_;

    friend class Planet;
};
}
