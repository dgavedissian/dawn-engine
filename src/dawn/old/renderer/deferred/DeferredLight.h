/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "OgreSimpleRenderable.h"
#include "MaterialGenerator.h"

namespace dw {

class DeferredLightRenderOperation;

// Deferred light geometry. Each instance matches a normal light. Should not be created by the user.
class DW_API DeferredLight : public Ogre::SimpleRenderable {
public:
    DeferredLight(MaterialGenerator* gen, Ogre::Light* parentLight,
                  DeferredLightRenderOperation* renderOp);
    ~DeferredLight();

    // Update the information from the light that matches this one
    void updateFromParent();

    // Update the information that is related to the camera
    void updateFromCamera(Ogre::Camera* camera);

    // Inherited from Ogre::SimpleRenderable
    virtual bool getCastChadows() const;
    virtual Ogre::Real getBoundingRadius() const override;
    virtual Ogre::Real getSquaredViewDepth(const Ogre::Camera*) const override;
    virtual const Ogre::MaterialPtr& getMaterial() const override;
    virtual void getWorldTransforms(Ogre::Matrix4* xform) const override;

protected:
    // Check if the camera is inside a light
    bool isCameraInside(Ogre::Camera* camera);

    // Set up geometry for this light
    void setupGeometry(float radius);

    // Set constant, linear, quadratic Attenuation terms
    void setAttenuation(float c, float b, float a);

    // Set the specular colour
    void setSpecularColour(const Ogre::ColourValue& col);

    // Parent RenderOperation
    DeferredLightRenderOperation* mLightRenderOp;

    // The light that this DeferredLight renders
    Ogre::Light* mParentLight;

    // Mode to ignore world orientation/position
    bool mIgnoreWorldTrans;

    // Light dimensions
    float mRadius;
    float mHeight;

    // Deferred shading system this math::Minilight is part of
    MaterialGenerator* mGenerator;

    // Material permutation
    Ogre::uint32 mPermutation;
};
}
