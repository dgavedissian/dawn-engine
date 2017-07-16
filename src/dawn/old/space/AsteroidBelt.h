/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

namespace dw {

class Renderer;
class SystemBody;

// Attached to a SystemBody - if the camera is located inside the belt then render asteroids
class DW_API AsteroidBelt {
public:
    AsteroidBelt(Renderer* rs, float minRadius, float maxRadius, float height);
    ~AsteroidBelt();

    // Update this asteroid belt
    void update(float dt, const Position& cameraPosition);

    // Pre-render this asteroid belt
    void preRender(Camera* camera);

    // Internal: Set parent
    void setParent(SystemBody* parent);

private:
    float mMinRadius;
    float mMaxRadius;
    float mHeight;

    struct Asteroid {
        Ogre::SceneNode* node;
        Position position;
    };

    Vector<Vector<Asteroid>> mAsteroidLevels;

    SystemBody* mParent;
};
}  // namespace dw
