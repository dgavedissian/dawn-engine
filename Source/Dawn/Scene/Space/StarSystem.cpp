/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#include "Common.h"
#include "Renderer/RenderSystem.h"
#include "Renderer/Deferred/DeferredShadingManager.h"
#include "Script/LuaState.h"
#include "StarSystem.h"

NAMESPACE_BEGIN

StarSystem::StarSystem(RenderSystem* rs, PhysicsManager* pm)
    : mRenderSystem(rs),
      mPhysicsManager(pm),
      mBackgroundSceneNode(nullptr)
{
    // Create the root scene node
    mBackgroundSceneNode = rs->GetRootSceneNode()->createChildSceneNode();

    // Set up the galactic plane
    mGalacticPlane = rs->GetSceneMgr()->createEntity(Ogre::SceneManager::PT_SPHERE);
    mGalacticPlane->setMaterialName("Scene/GalacticPlane");
    mGalacticPlane->setRenderQueueGroup(BACKGROUND_RENDER_QUEUE);
    mGalacticPlane->setCastShadows(false);
    mGalacticPlane->setQueryFlags(UNIVERSE_OBJECT);
    mBackgroundSceneNode->attachObject(mGalacticPlane);

    // Generate a random starfield
    mStarfield = rs->GetSceneMgr()->createEntity(Ogre::SceneManager::PT_CUBE);
    mStarfield->setMaterialName("Scene/Starfield");
    mStarfield->setRenderQueueGroup(BACKGROUND_RENDER_QUEUE);
    mStarfield->setCastShadows(false);
    mStarfield->setQueryFlags(UNIVERSE_OBJECT);
    mBackgroundSceneNode->attachObject(mStarfield);
}

StarSystem::~StarSystem()
{
    mRenderSystem->GetSceneMgr()->destroySceneNode(mBackgroundSceneNode);
    mRenderSystem->GetSceneMgr()->destroyEntity(mStarfield);
    mRenderSystem->GetSceneMgr()->destroyEntity(mGalacticPlane);
}

shared_ptr<AsteroidBelt> StarSystem::CreateAsteroidBelt(float minRadius, float maxRadius,
                                                        float height)
{
    shared_ptr<AsteroidBelt> asteroidBelt = make_shared<AsteroidBelt>(
        mRenderSystem, minRadius, maxRadius, height);
    mAsteroidBelts.push_back(asteroidBelt);
    return asteroidBelt;
}

shared_ptr<Barycentre> StarSystem::CreateBarycentre()
{
    shared_ptr<Barycentre> barycentre = make_shared<Barycentre>(mRenderSystem);
    mBodies.push_back(barycentre);
    return barycentre;
}

shared_ptr<Planet> StarSystem::CreatePlanet(PlanetDesc& desc)
{
    shared_ptr<Planet> planet = make_shared<Planet>(mRenderSystem, this, desc);
    mBodies.push_back(planet);
    return planet;
}

shared_ptr<Star> StarSystem::CreateStar(StarDesc& desc)
{
    shared_ptr<Star> star = make_shared<Star>(mRenderSystem, this, mPhysicsManager, desc);
    mBodies.push_back(star);
    mStars.push_back(star);
    return star;
}

void StarSystem::SetRoot(shared_ptr<SystemBody> body)
{
    mRootBody = body;
}

void StarSystem::CreateNebulaCloud(const string& texture, const Colour& fogColour)
{
    // Nebula Clouds
    mNebulaSceneNode = mBackgroundSceneNode->createChildSceneNode();
    mNebula = mRenderSystem->GetSceneMgr()->createEntity(Ogre::SceneManager::PT_SPHERE);
    mNebula->setMaterialName("Scene/Nebula");
    mNebula->setRenderQueueGroup(NEBULA_RENDER_QUEUE);
    mNebula->setCastShadows(false);
    mNebulaSceneNode->attachObject(mNebula);

    // Enable fog
    if (fogColour != Colour(0.0f, 0.0f, 0.0f, 0.0f))
    {
        mRenderSystem->GetDeferredShadingMgr()->EnableFog(true);

        // Set the fog colour
        Ogre::MaterialPtr material = mRenderSystem->GetMaterial("DeferredShading/Post/Fog");
    }
}

void StarSystem::Update(float dt)
{
    if (mRootBody)
        mRootBody->Update(dt, Position::origin);
}

void StarSystem::PreRender(Camera* camera)
{
    if (mRootBody)
        mRootBody->PreRender(camera);
}

void StarSystem::CalculatePosition(double time)
{
    if (mRootBody)
        mRootBody->CalculatePosition(time);
}

shared_ptr<SystemBody> StarSystem::GetRootBody()
{
    return mRootBody;
}

const vector<shared_ptr<SystemBody>>& StarSystem::GetBodies() const
{
    return mBodies;
}

const vector<shared_ptr<Star>>& StarSystem::GetStars() const
{
    return mStars;
}

Ogre::SceneNode* StarSystem::GetRootSceneNode()
{
    return mBackgroundSceneNode;
}

NAMESPACE_END
