/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "Base.h"
#include "scene/space/SystemBody.h"

namespace dw {
struct DW_API PlanetDesc {
    float radius = 0.0f;
    float rotationPeriod = 0.0f;    // in seconds
    float axialTilt = 0.0f;         // in radians
    String surfaceTexture = "";
    String nightTexture = "";

    bool hasAtmosphere = false;
    struct {
        float radius = 0.0f;
    } atmosphere;

    bool hasRings = false;
    struct {
        float minRadius = 0.0f;
        float maxRadius = 0.0f;
        String texture = "";
    } rings;

    PlanetDesc() = default;
};

// A planetary body which can have an atmosphere or ring system
class DW_API Planet : public SystemBody {
public:
    Planet(StarSystem* starSystem, PlanetDesc& desc);
    virtual ~Planet();

    // Accessors
    DEPRECATED Ogre::SceneNode* getSurfaceNode();

    // Inherited from SystemBody
    virtual void preRender(Camera* camera) override;
    virtual void calculatePosition(double time) override;

private:
    StarSystem* mStarSystem;

    PlanetDesc mDesc;

    Ogre::MeshPtr mSurfaceMesh;
    Ogre::MeshPtr mAtmosphereMesh;

    // Surface
    Quat mAxialTilt;
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
};
}
