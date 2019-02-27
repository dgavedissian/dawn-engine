/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "renderer/api/Renderer.h"
#include "AsteroidBelt.h"
#include "SystemBody.h"

#define ASTEROID_MESH_COUNT 3
#define ASTEROID_LEVELS 3
#define ASTEROID_COUNT 750
#define HALF_REGION_SIZE 4000.0f  // region size for min level
#define SCALE_FACTOR 4.0f
#define INITIAL_SIZE 6.0f

namespace dw {

AsteroidBelt::AsteroidBelt(Renderer* rs, float minRadius, float maxRadius, float height)
    : mMinRadius(minRadius), mMaxRadius(maxRadius), mHeight(height) {
    assert(mMinRadius < mMaxRadius);
    assert(mHeight > 0.0f);

    // Cache asteroids
    std::default_random_engine engine;
    std::uniform_int_distribution<int> randomID(1, ASTEROID_MESH_COUNT);
    std::uniform_real_distribution<float> randomRotation(-math::pi, math::pi);
    for (int l = 0; l < ASTEROID_LEVELS; ++l) {
        float posScaleFactor = math::PowInt(SCALE_FACTOR, l * 2);
        float sizeScaleFactor = math::PowInt(SCALE_FACTOR, l * 2);
        Vector<Asteroid> level;
        for (int i = 0; i < ASTEROID_COUNT; ++i) {
            Asteroid a;
            Ogre::Entity* ent = rs->getSceneMgr()->createEntity(
                "scene-asteroid" + std::to_string(randomID(engine)) + ".mesh");
            ent->setMaterialName("Scene/Asteroid");
            a.node = rs->getSceneMgr()->createSceneNode();
            a.node->attachObject(ent);
            a.node->setScale(Ogre::Vector3(INITIAL_SIZE * sizeScaleFactor));
            a.node->setOrientation(Quat::RotateX(randomRotation(engine)) *
                                   Quat::RotateY(randomRotation(engine)));
            rs->getRootSceneNode()->addChild(a.node);

            // Choose random position
            float halfRegionSize = HALF_REGION_SIZE * posScaleFactor;
            std::uniform_real_distribution<float> randomDist(-halfRegionSize, halfRegionSize);
            a.position.x = randomDist(engine);
            a.position.y = randomDist(engine);
            a.position.z = randomDist(engine);

            // Push
            level.push_back(a);
        }

        mAsteroidLevels.push_back(level);
    }
}

AsteroidBelt::~AsteroidBelt() {
}

void AsteroidBelt::update(float dt, const Position& cameraPosition) {
    if (mParent) {
        // Calculate position relative to belt origin
        Position relativePosition = cameraPosition - mParent->getPosition();

        // Determine whether the camera is within the bounds of the belt
        // TODO: stop wrapping asteroids if they're outside the boundaries
        {
            // Calculate thickness
            // TODO: Attenuate asteroids based on thickness

            // Update asteroids
            for (int l = 0; l < ASTEROID_LEVELS; ++l) {
                float scaleFactor = math::Pow(SCALE_FACTOR, (float)l);
                Vec3 halfRegion(HALF_REGION_SIZE * scaleFactor);
                Position min = relativePosition - Position(halfRegion);
                Position max = relativePosition + Position(halfRegion);
                for (auto& a : mAsteroidLevels[l]) {
                    a.position.x = wrap(a.position.x, min.x, max.x);
                    a.position.y = wrap(a.position.y, min.y, max.y);
                    a.position.z = wrap(a.position.z, min.z, max.z);
                }
            }
        }
    }
}

void AsteroidBelt::preRender(Camera* camera) {
    // Update asteroid meshes
    for (int l = 0; l < ASTEROID_LEVELS; ++l) {
        for (auto& a : mAsteroidLevels[l])
            a.node->setPosition(a.position.toCameraSpace(camera));
    }
}

void AsteroidBelt::setParent(SystemBody* parent) {
    mParent = parent;
}
}  // namespace dw
