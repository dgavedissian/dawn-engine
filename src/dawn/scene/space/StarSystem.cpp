/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#include "Common.h"
#include "Renderer/Renderer.h"
#include "Renderer/Deferred/DeferredShadingManager.h"
#include "Script/LuaState.h"
#include "StarSystem.h"

NAMESPACE_BEGIN

StarSystem::StarSystem(Renderer* rs, PhysicsWorld* pm)
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

SharedPtr<AsteroidBelt> StarSystem::CreateAsteroidBelt(float minRadius, float maxRadius,
                                                        float height)
{
    SharedPtr<AsteroidBelt> asteroidBelt = MakeShared<AsteroidBelt>(
        mRenderSystem, minRadius, maxRadius, height);
    mAsteroidBelts.push_back(asteroidBelt);
    return asteroidBelt;
}

SharedPtr<Barycentre> StarSystem::CreateBarycentre()
{
    SharedPtr<Barycentre> barycentre = MakeShared<Barycentre>(mRenderSystem);
    mBodies.push_back(barycentre);
    return barycentre;
}

SharedPtr<Planet> StarSystem::CreatePlanet(PlanetDesc& desc)
{
    SharedPtr<Planet> planet = MakeShared<Planet>(mRenderSystem, this, desc);
    mBodies.push_back(planet);
    return planet;
}

SharedPtr<Star> StarSystem::CreateStar(StarDesc& desc)
{
    SharedPtr<Star> star = MakeShared<Star>(mRenderSystem, this, mPhysicsManager, desc);
    mBodies.push_back(star);
    mStars.push_back(star);
    return star;
}

void StarSystem::SetRoot(SharedPtr<SystemBody> body)
{
    mRootBody = body;
}

void StarSystem::CreateNebulaCloud(const String& texture, const Colour& fogColour)
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

SharedPtr<SystemBody> StarSystem::GetRootBody()
{
    return mRootBody;
}

const Vector<SharedPtr<SystemBody>>& StarSystem::GetBodies() const
{
    return mBodies;
}

const Vector<SharedPtr<Star>>& StarSystem::GetStars() const
{
    return mStars;
}

Ogre::SceneNode* StarSystem::GetRootSceneNode()
{
    return mBackgroundSceneNode;
}

NAMESPACE_END
