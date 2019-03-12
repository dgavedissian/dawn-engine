/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "renderer/api/Renderer.h"
#include "renderer/deferred/DeferredShadingManager.h"
#include "script/LuaState.h"
#include "StarSystem.h"

namespace dw {

StarSystem::StarSystem(Renderer* rs, PhysicsWorld* pm)
    : mRenderSystem(rs), mPhysicsManager(pm), mBackgroundSceneNode(nullptr) {
    // Create the root scene node
    mBackgroundSceneNode = rs->getRootSceneNode()->createChildSceneNode();

    // Set up the galactic plane
    mGalacticPlane = rs->getSceneMgr()->createEntity(Ogre::SceneManager::PT_SPHERE);
    mGalacticPlane->setMaterialName("Scene/GalacticPlane");
    mGalacticPlane->setRenderQueueGroup(BACKGROUND_RENDER_QUEUE);
    mGalacticPlane->setCastShadows(false);
    mGalacticPlane->setQueryFlags(UNIVERSE_OBJECT);
    mBackgroundSceneNode->attachObject(mGalacticPlane);

    // Generate a random starfield
    mStarfield = rs->getSceneMgr()->createEntity(Ogre::SceneManager::PT_CUBE);
    mStarfield->setMaterialName("Scene/Starfield");
    mStarfield->setRenderQueueGroup(BACKGROUND_RENDER_QUEUE);
    mStarfield->setCastShadows(false);
    mStarfield->setQueryFlags(UNIVERSE_OBJECT);
    mBackgroundSceneNode->attachObject(mStarfield);
}

StarSystem::~StarSystem() {
    mRenderSystem->getSceneMgr()->destroySceneNode(mBackgroundSceneNode);
    mRenderSystem->getSceneMgr()->destroyEntity(mStarfield);
    mRenderSystem->getSceneMgr()->destroyEntity(mGalacticPlane);
}

SharedPtr<AsteroidBelt> StarSystem::createAsteroidBelt(float minRadius, float maxRadius,
                                                       float height) {
    SharedPtr<AsteroidBelt> asteroidBelt =
        makeShared<AsteroidBelt>(mRenderSystem, minRadius, maxRadius, height);
    mAsteroidBelts.push_back(asteroidBelt);
    return asteroidBelt;
}

SharedPtr<Barycentre> StarSystem::createBarycentre() {
    SharedPtr<Barycentre> barycentre = makeShared<Barycentre>(mRenderSystem);
    mBodies.push_back(barycentre);
    return barycentre;
}

SharedPtr<Planet> StarSystem::createPlanet(PlanetDesc& desc) {
    SharedPtr<Planet> planet = makeShared<Planet>(mRenderSystem, this, desc);
    mBodies.push_back(planet);
    return planet;
}

SharedPtr<Star> StarSystem::createStar(StarDesc& desc) {
    SharedPtr<Star> star = makeShared<Star>(mRenderSystem, this, mPhysicsManager, desc);
    mBodies.push_back(star);
    mStars.push_back(star);
    return star;
}

void StarSystem::setRoot(SharedPtr<SystemBody> body) {
    mRootBody = body;
}

void StarSystem::createNebulaCloud(const String& texture, const Colour& fogColour) {
    // Nebula Clouds
    mNebulaSceneNode = mBackgroundSceneNode->createChildSceneNode();
    mNebula = mRenderSystem->getSceneMgr()->createEntity(Ogre::SceneManager::PT_SPHERE);
    mNebula->setMaterialName("Scene/Nebula");
    mNebula->setRenderQueueGroup(NEBULA_RENDER_QUEUE);
    mNebula->setCastShadows(false);
    mNebulaSceneNode->attachObject(mNebula);

    // Enable fog
    if (fogColour != Colour(0.0f, 0.0f, 0.0f, 0.0f)) {
        mRenderSystem->getDeferredShadingMgr()->EnableFog(true);

        // Set the fog colour
        Ogre::MaterialPtr material = mRenderSystem->getMaterial("DeferredShading/Post/Fog");
    }
}

void StarSystem::update(float dt) {
    if (mRootBody)
        mRootBody->update(dt, Position::origin);
}

void StarSystem::preRender(Camera* camera) {
    if (mRootBody)
        mRootBody->preRender(camera);
}

void StarSystem::calculatePosition(double time) {
    if (mRootBody)
        mRootBody->calculatePosition(time);
}

SharedPtr<SystemBody> StarSystem::getRootBody() {
    return mRootBody;
}

const Vector<SharedPtr<SystemBody>>& StarSystem::getBodies() const {
    return mBodies;
}

const Vector<SharedPtr<Star>>& StarSystem::getStars() const {
    return mStars;
}

Ogre::SceneNode* StarSystem::getRootSceneNode() {
    return mBackgroundSceneNode;
}
}  // namespace dw
