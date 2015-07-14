/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
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
    shared_ptr<AsteroidBelt> CreateAsteroidBelt(float minRadius, float maxRadius, float height);

    // Create a bary centre
    shared_ptr<Barycentre> CreateBarycentre();

    // Create a planet
    shared_ptr<Planet> CreatePlanet(PlanetDesc& desc);

    // Create a star
    shared_ptr<Star> CreateStar(StarDesc& desc);

    // Set root object
    void SetRoot(shared_ptr<SystemBody> body);

    // Nebula Cloud
    void CreateNebulaCloud(const string& texture, const Colour& fogColour);

    // Updating
    void Update(float dt);

    // Pre-render
    void PreRender(Camera* camera);

    // Calculate the position of each object in the system at a particular time in seconds
    void CalculatePosition(double time);

    // Accessors
    shared_ptr<SystemBody> GetRootBody();
    const vector<shared_ptr<SystemBody>>& GetBodies() const;
    const vector<shared_ptr<Star>>& GetStars() const;
    DEPRECATED Ogre::SceneNode* GetRootSceneNode();

private:
    Renderer* mRenderSystem;
    PhysicsWorld* mPhysicsManager;

    shared_ptr<SystemBody> mRootBody;
    vector<shared_ptr<SystemBody>> mBodies;
    vector<shared_ptr<Star>> mStars;
    vector<shared_ptr<AsteroidBelt>> mAsteroidBelts;

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
