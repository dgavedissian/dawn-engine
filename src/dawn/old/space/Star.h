/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "SystemBody.h"

namespace dw {

class Renderer;
class StarSystem;
class PhysicsWorld;

enum SpectralClass { SC_O, SC_B, SC_A, SC_F, SC_G, SC_K, SC_M };

struct DW_API StarDesc {
    float radius;
    SpectralClass spectralClass;

    StarDesc();
};

class DW_API Star : public SystemBody {
public:
    Star(Renderer* renderSystem, StarSystem* starSystem, PhysicsWorld* physicsMgr, StarDesc& desc);
    virtual ~Star();

    // Get the stars colour
    Colour getColour() const;

    // Inherited from SystemBody
    void preRender(Camera* camera) override;

private:
    PhysicsWorld* mPhysicsMgr;
    StarDesc mDesc;

    // Flare
    float mFlareVisibility;
    Ogre::Billboard* mFlareBillboard;
    Ogre::Billboard* mGlowBillboard;

    // TODO: Move BillboardSet's to the Galaxy root class
    Ogre::BillboardSet* mFlareBillboardSet;
    Ogre::BillboardSet* mGlowBillboardSet;
    Ogre::SceneNode* mSceneNode;

    // Light
    Ogre::Light* mLight;
};
}
