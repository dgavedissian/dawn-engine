/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "core/Utils.h"
#include "renderer/Renderer.h"
#include "scene/PhysicsScene.h"
#include "StarSystem.h"
#include "Planet.h"
#include "Star.h"

namespace dw {

PlanetDesc::PlanetDesc() {
    radius = 0.0f;
    rotationPeriod = 0.0f;
    axialTilt = 0.0f;
    surfaceTexture = "";
    nightTexture = "";
    hasAtmosphere = false;
    atmosphere.radius = 0.0f;
    hasRings = false;
    rings.minRadius = 0.0f;
    rings.maxRadius = 0.0f;
    rings.texture = "";
}

Planet::Planet(Renderer* renderSystem, StarSystem* starSystem, PlanetDesc& desc)
    : SystemBody(renderSystem),
      mStarSystem(starSystem),
      mDesc(desc),
      mAxialTilt(Vec3(0.0f, 0.0f, 1.0f), -mDesc.axialTilt),
      mSurfaceEntity(nullptr),
      mSurfaceNode(nullptr),
      mAtmosphereEntity(nullptr),
      mAtmosphereNode(nullptr) {
    assert(desc.radius > 0.0f);

    // Create the surface
    mSurfaceMesh = createSphere(generateName("Sphere"), desc.radius, 64, 64, true, true);
    mSurfaceMaterial =
        renderSystem->getMaterialCopy("Scene/Planet", generateName("planetMaterial"));
    mSurfaceEntity = renderSystem->getSceneMgr()->createEntity(mSurfaceMesh);
    mSurfaceEntity->setMaterial(mSurfaceMaterial);
    // mSurfaceEntity->setRenderQueueGroup(PLANET_SURFACE_RENDER_QUEUE);
    mSurfaceEntity->setQueryFlags(UNIVERSE_OBJECT);
    mSurfaceSubEntity = mSurfaceEntity->getSubEntity(0);
    mSurfaceNode = renderSystem->getRootSceneNode()->createChildSceneNode();
    mSurfaceNode->attachObject(mSurfaceEntity);

    // Set the surface shader properties
    Ogre::Pass* pass = mSurfaceMaterial->getTechnique(0)->getPass(0);
    pass->getTextureUnitState("surface")->setTextureName(desc.surfaceTexture);
    pass->getTextureUnitState("surface")->setHardwareGammaEnabled(true);
    pass->getTextureUnitState("night")->setTextureName(desc.nightTexture);
    pass->getTextureUnitState("night")->setHardwareGammaEnabled(true);

    // Create the atmosphere
    if (desc.hasAtmosphere) {
        mAtmosphereMesh =
            createSphere(generateName("Sphere"), desc.atmosphere.radius, 80, 80, true, true);
        mAtmosphereMaterial =
            renderSystem->getMaterialCopy("Scene/Atmosphere", generateName("atmosphereMaterial"));
        mAtmosphereEntity = renderSystem->getSceneMgr()->createEntity(mAtmosphereMesh);
        mAtmosphereEntity->setMaterial(mAtmosphereMaterial);
        mAtmosphereEntity->setRenderQueueGroup(ATMOSPHERE_RENDER_QUEUE);
        mAtmosphereEntity->setQueryFlags(UNIVERSE_OBJECT);
        mAtmosphereSubEntity = mAtmosphereEntity->getSubEntity(0);
        mAtmosphereNode = renderSystem->getRootSceneNode()->createChildSceneNode();
        mAtmosphereNode->attachObject(mAtmosphereEntity);

        // Compute and set shader constants
        Vec3 wavelength(0.65f, 0.57f, 0.475f);  // 650nm for red, 570mn for green, 475nm for blue
        Vec3 invWavelength(1.0f / pow(wavelength.x, 4.0f), 1.0f / pow(wavelength.y, 4.0f),
                           1.0f / pow(wavelength.z, 4.0f));
        float Kr = 0.0025f, Km = 0.0015f, ESun = 15.0f,
              scale = 1.0f / (desc.atmosphere.radius - desc.radius), scaleDepth = 0.25f, g = -0.95f;
        mSurfaceSubEntity->setCustomParameter(
            5, Ogre::Vector4(invWavelength.x, invWavelength.y, invWavelength.z, 0.0f));
        mSurfaceSubEntity->setCustomParameter(
            6, Ogre::Vector4(desc.radius, desc.radius * desc.radius, desc.atmosphere.radius,
                             desc.atmosphere.radius * desc.atmosphere.radius));
        mSurfaceSubEntity->setCustomParameter(
            7, Ogre::Vector4(Kr * ESun, Kr * 4.0f * math::pi, Km * ESun, Km * 4.0f * math::pi));
        mSurfaceSubEntity->setCustomParameter(
            8, Ogre::Vector4(scale, scaleDepth, scale / scaleDepth, 0.0f));

        mAtmosphereSubEntity->setCustomParameter(
            0, Ogre::Vector4(invWavelength.x, invWavelength.y, invWavelength.z, 0.0f));
        mAtmosphereSubEntity->setCustomParameter(
            1, Ogre::Vector4(desc.radius, desc.radius * desc.radius, desc.atmosphere.radius,
                             desc.atmosphere.radius * desc.atmosphere.radius));
        mAtmosphereSubEntity->setCustomParameter(
            2, Ogre::Vector4(Kr * ESun, Kr * 4.0f * math::pi, Km * ESun, Km * 4.0f * math::pi));
        mAtmosphereSubEntity->setCustomParameter(
            3, Ogre::Vector4(scale, scaleDepth, scale / scaleDepth, 0.0f));
        mAtmosphereSubEntity->setCustomParameter(7, Ogre::Vector4(g, g * g, 0.0f, 0.0f));
    }

    // Create the ring system
    if (desc.hasRings) {
        mRingSystem.reset(new Rings(desc, this));

        // Set ring system properties on the surface shader
        Ogre::Vector4 ringDimensions(desc.rings.minRadius, desc.rings.maxRadius,
                                     desc.rings.maxRadius - desc.rings.minRadius, 0.0f);
        Ogre::Pass* surfacePass = mSurfaceMaterial->getTechnique(0)->getPass(0);
        surfacePass->getTextureUnitState("rings")->setTextureName(desc.rings.texture);
        surfacePass->getTextureUnitState("rings")->setHardwareGammaEnabled(true);
        mSurfaceSubEntity->setCustomParameter(4, ringDimensions);
    }

    // Configure planet shader
    mSurfaceSubEntity->setCustomParameter(
        0, Ogre::Vector4(desc.nightTexture.length() > 0 ? 1.0f : 0.0f, desc.hasAtmosphere,
                         desc.hasRings, 0.0f));
}

Planet::~Planet() {
    mRingSystem.reset();

    if (mAtmosphereEntity) {
        mRenderSystem->getSceneMgr()->destroySceneNode(mAtmosphereNode);
        mRenderSystem->getSceneMgr()->destroyEntity(mAtmosphereEntity);
        mAtmosphereMaterial.setNull();
        mAtmosphereMesh.setNull();
    }

    mRenderSystem->getSceneMgr()->destroySceneNode(mSurfaceNode);
    mRenderSystem->getSceneMgr()->destroyEntity(mSurfaceEntity);
}

Ogre::SceneNode* Planet::getSurfaceNode() {
    return mSurfaceNode;
}

void Planet::preRender(Camera* camera) {
    Vec3 cameraSpacePosition = mPosition.toCameraSpace(camera);

    // Scale the surface and atmosphere
    mSurfaceNode->setPosition(cameraSpacePosition);
    if (mDesc.hasAtmosphere)
        mAtmosphereNode->setPosition(mSurfaceNode->getPosition());

    // Detect a sun object
    // TODO support multiple stars
    if (mStarSystem->getStars().size() > 0) {
        Position sunPosition = (*mStarSystem->getStars().begin())->getPosition();
        Vec3 sunDirection = -mPosition.getRelativeTo(sunPosition).Normalized();
        Vec3 localSunDirection = mAxialTilt.Inverted() * sunDirection;

        // Set the direction in the planet shader
        mSurfaceSubEntity->setCustomParameter(1, Ogre::Vector4(mSurfaceNode->getPosition()));
        mSurfaceSubEntity->setCustomParameter(2, Ogre::Vector4(sunDirection));
        mSurfaceSubEntity->setCustomParameter(3, Ogre::Vector4(localSunDirection));

        // Set the direction in the atmosphere shader
        if (mDesc.hasAtmosphere)
            mAtmosphereEntity->getSubEntity(0)->setCustomParameter(
                4, Ogre::Vector4(sunDirection.x, sunDirection.y, sunDirection.z, 0.0f));

        // Update ring shader parameters
        if (mRingSystem) {
            // Update ring dimensions
            Ogre::Vector4 ringDimensions(
                Ogre::Vector3(mDesc.rings.minRadius, mDesc.rings.maxRadius,
                              mDesc.rings.maxRadius - mDesc.rings.minRadius));
            mSurfaceSubEntity->setCustomParameter(4, ringDimensions);

            // Convert light direction into a position by moving it from the planets
            // position along the direction vector by a large number.
            // This is somewhat hacky but it works
            Vec3 sunPosition = mSurfaceNode->getPosition() + sunDirection * 1e9f;
            Vec3 sunToPlanet = mSurfaceNode->getPosition() - sunPosition;
            Ogre::SubEntity* se = mRingSystem->mRingEntity->getSubEntity(0);
            se->setCustomParameter(0, Ogre::Vector4(mDesc.radius, 0.0f, 0.0f, 0.0f));
            se->setCustomParameter(1, Ogre::Vector4(ringDimensions));
            se->setCustomParameter(2, Ogre::Vector4(mSurfaceNode->getPosition()));
            se->setCustomParameter(3, Ogre::Vector4(1.0f / (mDesc.atmosphere.radius - mDesc.radius),
                                                    0.0f, 0.0f, 0.0f));
            se->setCustomParameter(5, Ogre::Vector4(sunToPlanet.x, sunToPlanet.y, sunToPlanet.z,
                                                    sunToPlanet.Length()));
            se->setCustomParameter(6, Ogre::Vector4(sunPosition));
        }
    }

    // Updates based on camera position
    //===========================================================================
    Vec3 localCameraPosition = -cameraSpacePosition;

    // Update rings
    if (mDesc.hasRings)
        mRingSystem->update(mSurfaceNode->getOrientation().Inverse() * localCameraPosition);

    // Update atmosphere shader
    if (mDesc.hasAtmosphere) {
        float camHeight = localCameraPosition.Length();
        mSurfaceSubEntity->setCustomParameter(
            10, Ogre::Vector4(localCameraPosition.x, localCameraPosition.y, localCameraPosition.z,
                              0.0f));
        mSurfaceSubEntity->setCustomParameter(
            11, Ogre::Vector4(camHeight, camHeight * camHeight, 0.0f, 0.0f));
        mAtmosphereSubEntity->setCustomParameter(
            5, Ogre::Vector4(localCameraPosition.x, localCameraPosition.y, localCameraPosition.z,
                             0.0f));
        mAtmosphereSubEntity->setCustomParameter(
            6, Ogre::Vector4(camHeight, camHeight * camHeight, 0.0f, 0.0f));
    }

    // Pre-render
    SystemBody::preRender(camera);
}

void Planet::calculatePosition(double time) {
    // TODO Rotating the planet causes issues with the atmosphere shader
    // mSurfaceNode->setOrientation(mAxialTilt * Quat(Vec3::unitY, time / mDesc.rotationPeriod *
    // 2.0f * math::pi));
    SystemBody::calculatePosition(time);
}

Planet::Rings::Rings(PlanetDesc& desc, Planet* parent)
    : mMinRadius(desc.rings.minRadius), mMaxRadius(desc.rings.maxRadius), mParent(parent) {
    String ringMeshName = generateName("ringsMesh");

    // Generate the LOD look up table
    mLodDistanceTable[0] = pow(mMaxRadius * 2.0f, 2);
    for (int i = 1; i < RING_DETAIL_MAX_LEVEL; i++) {
        mLodDistanceTable[i] = mLodDistanceTable[i - 1] * 0.25f;
    }

    // Generate a List of points used by the ring mesh
    uint detail = 16;
    Vector<Vec2> vertices;
    float offset = math::Cos(math::pi / static_cast<float>(detail));
    float maxRadius = desc.rings.maxRadius / offset;
    float minRadius = desc.rings.minRadius * offset;
    for (uint16_t i = 0; i < detail; ++i) {
        float angle = ((float)i / (float)detail) * math::pi * 2.0f;
        Vec2 pos(sin(angle), cos(angle));
        vertices.push_back(pos * maxRadius);
        vertices.push_back(pos * minRadius);
    }

    // Create the mesh from the data
    Ogre::ManualObject* ringMesh = parent->mRenderSystem->getSceneMgr()->createManualObject();
    ringMesh->estimateVertexCount(vertices.size());
    ringMesh->begin("");

    // Add the vertices
    for (uint i = 0; i < vertices.size(); ++i) {
        Ogre::Vector3 transformedPoint(vertices[i].x, 0.0f, vertices[i].y);
        ringMesh->position(transformedPoint);
    }

    // Add the indices
    for (uint i = 0; i < detail; ++i) {
        int startIndex = i * 2;

        if (i < (detail - 1)) {
            ringMesh->index(startIndex);
            ringMesh->index(startIndex + 2);
            ringMesh->index(startIndex + 1);
            ringMesh->index(startIndex + 2);
            ringMesh->index(startIndex + 3);
            ringMesh->index(startIndex + 1);
        } else {
            ringMesh->index(startIndex);
            ringMesh->index(0);
            ringMesh->index(startIndex + 1);
            ringMesh->index(0);
            ringMesh->index(1);
            ringMesh->index(startIndex + 1);
        }
    }

    // Finish adding vertices
    ringMesh->end();

    // Convert this manual object into a mesh and free the buffers
    mRingMesh = ringMesh->convertToMesh(ringMeshName);
    parent->mRenderSystem->getSceneMgr()->destroyManualObject(ringMesh);

    // Create the entity and attach
    mRingMaterial =
        parent->mRenderSystem->getMaterialCopy("Scene/Rings", generateName("ringsMaterial"));
    Ogre::Pass* ringPass = mRingMaterial->getTechnique(0)->getPass(0);
    ringPass->getTextureUnitState("rings")->setTextureName(desc.rings.texture);
    ringPass->getTextureUnitState("rings")->setHardwareGammaEnabled(true);
    mRingEntity = parent->mRenderSystem->getSceneMgr()->createEntity(mRingMesh);
    mRingEntity->setMaterial(mRingMaterial);
    mRingEntity->setQueryFlags(UNIVERSE_OBJECT);
    parent->getSurfaceNode()->attachObject(mRingEntity);

    // Load the texture
    mRingTexture.load(desc.rings.texture, "General");

    // Update shader parameters
    float minDistance = sqrt(mLodDistanceTable[RING_DETAIL_MAX_LEVEL - 6]);
    mRingEntity->getSubEntity(0)->setCustomParameter(0,
                                                     Ogre::Vector4(desc.radius, 0.0f, 0.0f, 0.0f));
    mRingEntity->getSubEntity(0)->setCustomParameter(
        1, Ogre::Vector4(desc.rings.minRadius, desc.rings.maxRadius,
                         desc.rings.maxRadius - desc.rings.minRadius, 0.0f));
    mRingEntity->getSubEntity(0)->setCustomParameter(
        3, Ogre::Vector4(1.0f / (desc.atmosphere.radius - desc.radius), 0.0f, 0.0f, 0.0f));
    mRingEntity->getSubEntity(0)->setCustomParameter(4, Ogre::Vector4(1.0f, 0.0f, 0.0f, 0.0f));
    mRingEntity->getSubEntity(0)->setCustomParameter(
        7, Ogre::Vector4(minDistance * 0.03125f, minDistance, 0.0f, 0.0f));

    // Set up the random number generator - doesn't matter what this value is as long as it never
    // changes
    mRandomGenerator.seed(0xDEADBEEF);

    // Set up the billboard set
    mParticlesLarge = mParent->mRenderSystem->getSceneMgr()->createBillboardSet(100000);
    mParticlesLarge->setMaterialName("Scene/Rings/Dust");
    mParticlesLarge->setQueryFlags(UNIVERSE_OBJECT);
    parent->getSurfaceNode()->attachObject(mParticlesLarge);
    mParticlesSmall = mParent->mRenderSystem->getSceneMgr()->createBillboardSet(100000);
    mParticlesSmall->setMaterialName("Scene/Rings/Dust");
    mParticlesLarge->setQueryFlags(UNIVERSE_OBJECT);
    parent->getSurfaceNode()->attachObject(mParticlesSmall);

    // Create the root ring node
    // x--------x
    // |        |
    // |        |  width & height = maxRadius * 2
    // |        |
    // x--------x
    mDetailRootNode =
        new RingNode(Vec2(-mMaxRadius, mMaxRadius), Vec2(mMaxRadius, -mMaxRadius), 0, this);
}

Planet::Rings::~Rings() {
    delete mDetailRootNode;
}

void Planet::Rings::update(const Vec3& cameraPosition) {
    mDetailRootNode->update(cameraPosition);
}

float Planet::Rings::getLodDistance(uint level) const {
    assert(level < RING_DETAIL_MAX_LEVEL);
    return mLodDistanceTable[level];
}

Colour Planet::Rings::getColour(const Vec2& position) const {
    float distance = position.Length();
    float index = (distance - mMinRadius) / (mMaxRadius - mMinRadius);

    // Trim the outer parts of the ring.
    if (index < 0.0f || index > 1.0f) {
        return Colour::Black;
    }

    size_t scaledIndex = static_cast<size_t>(math::Floor(index * (float)mRingTexture.getWidth()));
    Colour colour = mRingTexture.getColourAt(scaledIndex, 0, 0);
    colour.a = 1.0f;
    return colour;
}

float Planet::Rings::getDensity(const Vec2& position) const {
    float distance = position.Length();
    float index = (distance - mMinRadius) / (mMaxRadius - mMinRadius);

    // Trim the outer parts of the ring.
    if (index < 0.0f || index > 1.0f) {
        return 0.0f;
    }

    size_t scaledIndex = static_cast<size_t>(math::Floor(index * (float)mRingTexture.getWidth()));
    return mRingTexture.getColourAt(scaledIndex, 0, 0).a;
}

float Planet::Rings::getThickness() const {
    return 1500.0f;
}

Planet::RingNode::RingNode(const Vec2& min, const Vec2& max, uint level, Rings* parent)
    : mMin(min),
      mMax(max),
      mCentre((min + max) * 0.5f),
      mLevel(level),
      mParentRingSystem(parent),
      mHasChildren(false) {
    cache();
}

Planet::RingNode::~RingNode() {
    free();
}

void Planet::RingNode::update(const Vec3& cameraPosition) {
    // Squared distance
    Vec3 diff = cameraPosition - Vec3(mCentre.x, 0.0f, mCentre.y);
    float distance = diff.Dot(diff);

    // Swap between billboards and meshes if they're within the right distance
    for (auto i = mAsteroidList.begin(); i != mAsteroidList.end(); i++) {
        Vec3 nodeDiff = cameraPosition - (*i).node->getPosition();
        float sqDistance = nodeDiff.Dot(nodeDiff);

        if (sqDistance < mParentRingSystem->getLodDistance(RING_DETAIL_MAX_LEVEL - 1)) {
            if (!(*i).node->isInSceneGraph()) {
                mParentRingSystem->mParent->getSurfaceNode()->addChild((*i).node);
                mParentRingSystem->mParticlesLarge->removeBillboard((*i).billboard);

                // Update the physical body
                /*
                PhysicsSystem::inst()._addToWorld((*i).rigidBody);
                btTransform& xform = (*i).rigidBody->getWorldTransform();
                xform.setOrigin(Vec3((*i).node->_getDerivedPosition()));
                xform.setRotation(Quat((*i).node->_getDerivedOrientation()));
                */
            }
        } else {
            if ((*i).node->isInSceneGraph()) {
                // PhysicsSystem::inst()._removeFromWorld((*i).rigidBody);
                mParentRingSystem->mParent->getSurfaceNode()->removeChild((*i).node);
                (*i).billboard = mParentRingSystem->mParticlesLarge->createBillboard(
                    (*i).node->getPosition(), (*i).colour);
                (*i).billboard->setDimensions((*i).size, (*i).size);
            }
        }
    }

    if (mHasChildren) {
        // Combine the node if it has children if the distance is too far
        if (distance > mParentRingSystem->getLodDistance(mLevel)) {
            join();
        } else {
            // Otherwise update...
            for (int i = 0; i < 4; i++) {
                mChildren[i]->update(cameraPosition);
            }
        }
    } else {
        // Split the node into 4 children if the distance threshold is reached
        if (distance < mParentRingSystem->getLodDistance(mLevel) &&
            mLevel < (RING_DETAIL_MAX_LEVEL - 1)) {
            split();

            // Update those children in case the camera has moved VERY quickly
            assert(mHasChildren == true);

            for (int i = 0; i < 4; i++) {
                mChildren[i]->update(cameraPosition);
            }
        }
    }
}

void Planet::RingNode::cache() {
    std::uniform_real_distribution<float> uniformDist(0.0f, 1.0f);
    std::uniform_int_distribution<int> asteroidTypeDistribution(1, 3);
    std::uniform_real_distribution<float> asteroidRotationDistribution(0.0f, math::pi);
    std::normal_distribution<float> asteroidDistribution(0.0f, 0.1f);
    std::normal_distribution<float> dustDistribution(0.0f, 0.1f);

    // Generate asteroids
    if (mLevel > 6) {
        int noAsteroids = static_cast<int>(math::Pow(1.7f, static_cast<float>(mLevel - 6))) * 2;
        float asteroidSize =
            mParentRingSystem->getThickness() * 0.4f / static_cast<float>(mLevel - 6);

        for (int i = 0; i < noAsteroids; i++) {
            // Generate position
            Vec3 position(
                math::Lerp(mMin.x, mMax.x, uniformDist(mParentRingSystem->mRandomGenerator)),
                mParentRingSystem->getThickness() *
                    asteroidDistribution(mParentRingSystem->mRandomGenerator),
                math::Lerp(mMin.y, mMax.y, uniformDist(mParentRingSystem->mRandomGenerator)));

            // Generate a random number in the range [0;1] and if that number is
            // less
            // than the density probability then spawn an asteroid there
            if (uniformDist(mParentRingSystem->mRandomGenerator) <
                mParentRingSystem->getDensity(Vec2(position.x, position.z))) {
                RingAsteroid asteroid;
                asteroid.size = asteroidSize;

                // Create a billboard
                asteroid.colour = mParentRingSystem->getColour(Vec2(position.x, position.z));
                asteroid.billboard =
                    mParentRingSystem->mParticlesLarge->createBillboard(position, asteroid.colour);
                asteroid.billboard->setDimensions(asteroidSize, asteroidSize);

                // Create the entity
                asteroid.node = nullptr;
                Ogre::Entity* entity =
                    mParentRingSystem->mParent->mRenderSystem->getSceneMgr()->createEntity(
                        "scene-asteroid" +
                        std::to_string(
                            asteroidTypeDistribution(mParentRingSystem->mRandomGenerator)) +
                        ".mesh");
                entity->setMaterialName("Scene/Rings/Asteroid");
                asteroid.node =
                    mParentRingSystem->mParent->mRenderSystem->getSceneMgr()->createSceneNode();
                asteroid.node->attachObject(entity);
                asteroid.node->setPosition(position);
                //        asteroid.node->setOrientation(
                //          glm::rotate(Quat(asteroidRotationDistribution(mParentRingSystem->mRandomGenerator))
                //          *
                //          Quat::RotateY(asteroidRotationDistribution(mParentRingSystem->mRandomGenerator)));
                asteroid.node->setScale(Ogre::Vector3(asteroidSize));

                // Create the rigid body
                /*
                Ogre::Matrix4 scaleTransform = Ogre::Matrix4::IDENTITY;
                scaleTransform.setScale(Vec3(asteroidSize));
                BtOgre::StaticMeshToShapeConverter entityCollisionMesh(entity,
                scaleTransform);
                asteroid.rigidBody = PhysicsSystem::inst().createRigidBody(0.0f,
                entityCollisionMesh.createConvex());
                */

                // Spawn an asteroid
                mAsteroidList.push_back(asteroid);
            }
        }
    }

    // Generate dust particles
    /*
    if (mLevel > 5)
    {
      int noDustParticles = (int)math::PowInt(2.0f, mLevel - 5) * 3;
      float dustParticleSize = mParentRingSystem->getThickness() * 0.05f /
    (float)(mLevel - 5);

      for (int i = 0; i < noDustParticles; i++)
      {
        // Generate position
        Vec3 position(math::Lerp(mMin.x, mMax.x,
    uniformDist(mParentRingSystem->mRandomGenerator)),
                      mParentRingSystem->getThickness() * 0.5f *
    dustDistribution(mParentRingSystem->mRandomGenerator),
                      math::Lerp(mMin.y, mMax.y,
    uniformDist(mParentRingSystem->mRandomGenerator)));

        // Generate a random number in the range [0;1] and if that number is
    less
    than the density probability then spawn an asteroid there
        if (uniformDist(mParentRingSystem->mRandomGenerator) <
    mParentRingSystem->getDensity(Ogre::Vector2(position.x, position.z)))
        {
          Colour colour =
    mParentRingSystem->getColour(Ogre::Vector2(position.x,
    position.z));
          Ogre::Billboard* billboard =
    mParentRingSystem->getSmallParticleSet()->createBillboard(position,
    colour);
          billboard->setDimensions(dustParticleSize, dustParticleSize);
          mDustList.push_back(billboard);
        }
      }
    }*/
}

void Planet::RingNode::free() {
    // Destroy billboards and meshes
    for (auto i = mAsteroidList.begin(); i != mAsteroidList.end(); i++) {
        // Only remove the billboard if it's being used instead of the mesh
        if (!(*i).node->isInSceneGraph()) {
            mParentRingSystem->mParticlesLarge->removeBillboard((*i).billboard);
        } else
            ;  // PhysicsSystem::inst()._removeFromWorld((*i).rigidBody);

        mParentRingSystem->mParent->mRenderSystem->getSceneMgr()->destroyEntity(
            static_cast<Ogre::Entity*>((*i).node->getAttachedObject(0)));
        mParentRingSystem->mParent->mRenderSystem->getSceneMgr()->destroySceneNode((*i).node);
        // PhysicsSystem::inst().destroyCollisionShape((*i).rigidBody->GetBulletCollisionShape());
        // PhysicsSystem::inst().destroyRigidBody((*i).rigidBody);
    }

    for (auto i = mDustList.begin(); i != mDustList.end(); i++) {
        mParentRingSystem->mParticlesSmall->removeBillboard((*i));
    }

    mAsteroidList.clear();
    mDustList.clear();

    // Destroy children
    if (mHasChildren) {
        join();
    }
}

void Planet::RingNode::split() {
    // TODO - pool RingNode's to save all these allocations!
    mChildren[0] = new RingNode(mMin, mCentre, mLevel + 1, mParentRingSystem);
    mChildren[1] = new RingNode(Vec2(mCentre.x, mMin.y), Vec2(mMax.x, mCentre.y), mLevel + 1,
                                mParentRingSystem);
    mChildren[2] = new RingNode(Vec2(mMin.x, mCentre.y), Vec2(mCentre.x, mMax.y), mLevel + 1,
                                mParentRingSystem);
    mChildren[3] = new RingNode(mCentre, mMax, mLevel + 1, mParentRingSystem);
    mHasChildren = true;
}

void Planet::RingNode::join() {
    for (int i = 0; i < 4; i++) {
        delete mChildren[i];
    }

    mHasChildren = false;
}
}  // namespace dw
