/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
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

namespace dw {

class Renderer;
class PhysicsWorld;

// TODO: Merge this into Galaxy?

// Handles a planetary system
class DW_API StarSystem {
public:
    StarSystem(Renderer* rs, PhysicsWorld* pm);
    ~StarSystem();

    // Create an asteroid belt
    SharedPtr<AsteroidBelt> createAsteroidBelt(float minRadius, float maxRadius, float height);

    // Create a bary centre
    SharedPtr<Barycentre> createBarycentre();

    // Create a planet
    SharedPtr<Planet> createPlanet(PlanetDesc& desc);

    // Create a star
    SharedPtr<Star> createStar(StarDesc& desc);

    // Set root object
    void setRoot(SharedPtr<SystemBody> body);

    // Nebula Cloud
    void createNebulaCloud(const String& texture, const Colour& fogColour);

    // Updating
    void update(float dt);

    // Pre-render
    void preRender(Camera* camera);

    // Calculate the position of each object in the system at a particular time in seconds
    void calculatePosition(double time);

    // Accessors
    SharedPtr<SystemBody> getRootBody();
    const Vector<SharedPtr<SystemBody>>& getBodies() const;
    const Vector<SharedPtr<Star>>& getStars() const;
    DEPRECATED Ogre::SceneNode* getRootSceneNode();

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
}  // namespace dw
