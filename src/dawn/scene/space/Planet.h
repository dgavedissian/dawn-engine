/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "SystemBody.h"

#define RING_DETAIL_MAX_LEVEL 14

NAMESPACE_BEGIN

class Renderer;
class StarSystem;

// Contains all the information needed to construct a planetary object
struct DW_API PlanetDesc
{
    float radius;
    float rotationPeriod;    // in seconds
    float axialTilt;         // in radians
    String surfaceTexture;
    String nightTexture;

    bool hasAtmosphere;
    struct
    {
        float radius;
    } atmosphere;

    bool hasRings;
    struct
    {
        float minRadius;
        float maxRadius;
        String texture;
    } rings;

    PlanetDesc();
};

// A planetary body which can have an atmosphere or ring system
class DW_API Planet : public SystemBody
{
public:
    Planet(Renderer* renderSystem, StarSystem* starSystem, PlanetDesc& desc);
    virtual ~Planet();

    // Accessors
    DEPRECATED Ogre::SceneNode* GetSurfaceNode();

    // Inherited from SystemBody
    virtual void PreRender(Camera* camera) override;
    virtual void CalculatePosition(double time) override;

private:
    class RingNode;

    class Rings
    {
    public:
        Rings(PlanetDesc& desc, Planet* parent);
        ~Rings();

        // Propagate updates to the detail hierarchy
        void Update(const Vec3& cameraPosition);    // Camera position must be in planet space

        // Get the level of detail distance boundary for a particular level
        float GetLodDistanceSq(uint level) const;
        Colour GetColour(const Vec2& position) const;    // texture colour component
        float GetDensity(const Vec2& position) const;    // texture alpha component

        // Accessors
        float GetThickness() const;

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
    struct RingAsteroid
    {
        Ogre::SceneNode* node;
        btRigidBody* rigidBody;
        Ogre::Billboard* billboard;
        float size;
        Colour colour;
    };

    // A planetary ring system detail node - handles a section of all the little
    // asteroids inside the ring system
    class RingNode
    {
    public:
        RingNode(const Vec2& min, const Vec2& max, uint level, Rings* parent);
        ~RingNode();

        // Check if this node needs to be joined/split. The camera position must be in camera space
        void Update(const Vec3& cameraPosition);

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
        void Cache();
        void Free();
        void Split();
        void Join();

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

NAMESPACE_END
