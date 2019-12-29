/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "Base.h"
#include "renderer/Material.h"
#include "scene/space/SystemBody.h"
#include "scene/space/PlanetRings.h"

namespace dw {
class StarSystem;

struct DW_API PlanetDesc {
    float radius = 0.0f;
    float rotational_period = 0.0f;  // in seconds
    float axial_tilt = 0.0f;         // in radians
    String surface_texture = "";
    String night_texture = "";
    String normal_map_texture = "";

    bool has_atmosphere = false;
    inline float atmosphere_radius() const {
        // The existing atmosphere shader assumes that the atmosphere sphere is exactly 2.5% larger.
        return radius * 1.025f;
    }

    bool has_rings = false;
    struct {
        float min_radius = 0.0f;
        float max_radius = 0.0f;
        String texture = "";
    } rings;

    PlanetDesc() = default;
};

// A planetary body which can have an atmosphere or ring system.
class DW_API Planet : public SystemBody {
public:
    DW_OBJECT(Planet);

    Planet(Context* ctx, SystemNode& system_node, StarSystem& star_system, const PlanetDesc& desc);
    ~Planet() override = default;

    // Inherited from SystemBody
    void preRender(Frame& frame, const Mat4& view_matrix, const Mat4& proj_matrix) override;
    void update(float dt, Frame& frame, const Vec3& camera_position) override;
    void updatePosition(double time) override;

private:
    StarSystem& star_system_;
    PlanetDesc desc_;

    Quat axial_tilt_;

    Vec3 sun_direction_;

    SharedPtr<Material> surface_material_;

    SystemNode* atmosphere_node_;
    SharedPtr<Material> atmosphere_material_;

    UniquePtr<PlanetRings> rings_;

    /*
    Ogre::MeshPtr mSurfaceMesh;
    Ogre::MeshPtr mAtmosphereMesh;

    // Surface
    Quat axial_tilt_;
    Ogre::MaterialPtr mSurfaceMaterial;
    Ogre::Entity* mSurfaceEntity;
    Ogre::SubEntity* mSurfaceSubEntity;
    Ogre::SceneNode* mSurfaceNode;

    // Atmosphere
    Ogre::MaterialPtr mAtmosphereMaterial;
    Ogre::Entity* mAtmosphereEntity;
    Ogre::SubEntity* mAtmosphereSubEntity;
    Ogre::SceneNode* mAtmosphereNode;

    // Ring System
    SharedPtr<Rings> mRingSystem;
     */
};
}  // namespace dw
