/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "Barycentre.h"
#include "Planet.h"
#include "Star.h"

// Rendering goes - back rings > atmosphere > surface > front rings > nebula
#define BACKGROUND_RENDER_QUEUE Ogre::RENDER_QUEUE_BACKGROUND
#define BACK_RINGS_RENDER_QUEUE (BACKGROUND_RENDER_QUEUE + 1)
#define ATMOSPHERE_RENDER_QUEUE (BACKGROUND_RENDER_QUEUE + 2)
#define PLANET_SURFACE_RENDER_QUEUE (BACKGROUND_RENDER_QUEUE + 3)
#define FRONT_RINGS_RENDER_QUEUE (BACKGROUND_RENDER_QUEUE + 4)
#define NEBULA_RENDER_QUEUE (BACKGROUND_RENDER_QUEUE + 5)

NAMESPACE_BEGIN

class Renderer;
class PhysicsWorld;

// TODO: Merge this into Galaxy?

// Handles a planetary system
class DW_API StarSystem
{
public:
    StarSystem(Renderer* rs, PhysicsWorld* pm);
    ~StarSystem();

    // Create an asteroid belt
    SharedPtr<AsteroidBelt> CreateAsteroidBelt(float minRadius, float maxRadius, float height);

    // Create a bary centre
    SharedPtr<Barycentre> CreateBarycentre();

    // Create a planet
    SharedPtr<Planet> CreatePlanet(PlanetDesc& desc);

    // Create a star
    SharedPtr<Star> CreateStar(StarDesc& desc);

    // Set root object
    void SetRoot(SharedPtr<SystemBody> body);

    // Nebula Cloud
    void CreateNebulaCloud(const String& texture, const Colour& fogColour);

    // Updating
    void Update(float dt);

    // Pre-render
    void PreRender(Camera* camera);

    // Calculate the position of each object in the system at a particular time in seconds
    void CalculatePosition(double time);

    // Accessors
    SharedPtr<SystemBody> GetRootBody();
    const Vector<SharedPtr<SystemBody>>& GetBodies() const;
    const Vector<SharedPtr<Star>>& GetStars() const;
    DEPRECATED Ogre::SceneNode* GetRootSceneNode();

private:
    Renderer* mRenderSystem;
    PhysicsWorld* mPhysicsManager;

    SharedPtr<SystemBody> mRootBody;
    Vector<SharedPtr<SystemBody>> mBodies;
    Vector<SharedPtr<Star>> mStars;
    Vector<SharedPtr<AsteroidBelt>> mAsteroidBelts;

    // Root
    Ogre::SceneNode* mBackgroundSceneNode;

    // Background
    Ogre::Entity* mGalacticPlane;
    Ogre::Entity* mStarfield;

    // Foreground nebula
    Ogre::SceneNode* mNebulaSceneNode;
    Ogre::Entity* mNebula;
};

NAMESPACE_END
