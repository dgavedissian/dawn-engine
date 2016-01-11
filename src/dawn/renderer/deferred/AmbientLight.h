/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

// Renderable for rendering Ambient component and also to
// establish the depths

// Just instantiation is sufficient
// Note that instantiation is necessary to at least establish the depths
// even if the current ambient colour is 0

// its ambient colour is same as the scene's ambient colour

// XXX Could make this a singleton/make it private to the DeferredShadingSystem
// e.g.

NAMESPACE_BEGIN

class DW_API AmbientLight : public Ogre::SimpleRenderable
{
public:
    AmbientLight();
    ~AmbientLight();

    // Inherited from Ogre::SimpleRenderable
    virtual Ogre::Real getBoundingRadius() const override;
    virtual Ogre::Real getSquaredViewDepth(const Ogre::Camera*) const override;
    virtual const Ogre::MaterialPtr& getMaterial() const override;
    virtual void getWorldTransforms(Ogre::Matrix4* xform) const override;
    void updateFromCamera(Ogre::Camera* camera);

protected:
    Ogre::Real mRadius;
    Ogre::MaterialPtr mMaterialPtr;
};

NAMESPACE_END
