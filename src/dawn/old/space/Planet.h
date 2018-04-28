/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "SystemBody.h"

#define RING_DETAIL_MAX_LEVEL 14

namespace dw {

class Renderer;
class StarSystem;

// Contains all the information needed to construct a planetary object
struct DW_API PlanetDesc {
    float radius;
    float rotationPeriod;  // in seconds
    float axialTilt;       // in radians
    String surfaceTexture;
    String nightTexture;

    bool hasAtmosphere;
    struct {
        float radius;
    } atmosphere;

    bool hasRings;
    struct {
        float minRadius;
        float maxRadius;
        String texture;
    } rings;

    PlanetDesc();
};

// A planetary body which can have an atmosphere or ring system
class DW_API Planet : public SystemBody {
public:
    Planet(Renderer* renderSystem, StarSystem* starSystem, PlanetDesc& desc);
    virtual ~Planet();

    // Accessors
    DEPRECATED Ogre::SceneNode* getSurfaceNode();

    // Inherited from SystemBody
    virtual void preRender(Camera* camera) override;
    virtual void calculatePosition(double time) override;

private:
    class RingNode;

    class Rings {
    public:
        Rings(PlanetDesc& desc, Planet* parent);
        ~Rings();

        // Propagate updates to the detail hierarchy
        void update(const Vec3& cameraPosition);  // Camera position must be in planet space

        // Get the level of detail distance boundary for a particular level
        float getLodDistance(uint level) const;
        Colour getColour(const Vec2& position) const;  // texture colour component
        float getDensity(const Vec2& position) const;  // texture alpha component

        // Accessors
        float getThickness() const;

    private:
        float mMinRadius;
        float mMaxRadius;
        Planet* mParent;

        Ogre::MeshPtr mRingMesh;
        Ogre::MaterialPtr mRingMaterial;
        Ogre::Image mRingTexture;
        Ogre::Entity* mRingEntity;

        // Ring detail
        std::default_random_engine mRandomGenerator;
        float mLodDistanceTable[RING_DETAIL_MAX_LEVEL];
        RingNode* mDetailRootNode;
        Ogre::BillboardSet* mParticlesSmall;
        Ogre::BillboardSet* mParticlesLarge;

        friend class Planet;
    };

    // A ring system asteroid
    struct RingAsteroid {
        Ogre::SceneNode* node;
        btRigidBody* rigidBody;
        Ogre::Billboard* billboard;
        float size;
        Colour colour;
    };

    // A planetary ring system detail node - handles a section of all the little
    // asteroids inside the ring system
    class RingNode {
    public:
        RingNode(const Vec2& min, const Vec2& max, uint level, Rings* parent);
        ~RingNode();

        // Check if this node needs to be joined/split. The camera position must be in camera space
        void update(const Vec3& cameraPosition);

    private:
        // Boundaries (in planet space)
        Vec2 mMin;
        Vec2 mMax;
        Vec2 mCentre;

        // Hierarchy
        uint mLevel;
        Rings* mParentRingSystem;

        // Child nodes
        bool mHasChildren;
        RingNode* mChildren[4];

        // Asteroids
        Vector<RingAsteroid> mAsteroidList;
        Vector<Ogre::Billboard*> mDustList;

        // Internal methods
        void cache();
        void free();
        void split();
        void join();

        friend class Rings;
    };

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
}  // namespace dw
