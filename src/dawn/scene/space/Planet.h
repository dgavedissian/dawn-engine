/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "Base.h"
#include "renderer/Material.h"
#include "scene/space/SystemBody.h"

namespace dw {
struct DW_API PlanetDesc {
    float radius = 0.0f;
    float rotational_period = 0.0f;    // in seconds
    float axial_tilt = 0.0f;         // in radians
    String surface_texture = "";
    String night_texture = "";
    String normal_map_texture = "base:space/moon_normal.png";

    bool has_atmosphere = false;
    struct {
        float radius = 0.0f;
    } atmosphere;

    bool has_rings = false;
    struct {
        float min_radius = 0.0f;
        float max_radius = 0.0f;
        String texture = "";
    } rings;

    PlanetDesc() = default;
};

// A planetary body which can have an atmosphere or ring system.
class StarSystem;
class DW_API Planet : public SystemBody {
public:
    DW_OBJECT(Planet);

    Planet(Context* ctx, SystemNode& system_node, StarSystem& star_system, const PlanetDesc& desc);
    ~Planet() override;

    // Inherited from SystemBody
    void preRender() override;
    void updatePosition(double time) override;

private:
    StarSystem& star_system_;
    PlanetDesc desc_;

    Quat axial_tilt_;

    SharedPtr<Material> surface_material_;

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
}
