/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "../GeometryUtils.h"
#include "AmbientLight.h"

namespace dw {

AmbientLight::AmbientLight() {
    // setRenderQueueGroup(Ogre::RENDER_QUEUE_2);

    mRenderOp.vertexData = new Ogre::VertexData();
    mRenderOp.indexData = nullptr;

    createQuad(mRenderOp.vertexData);

    mRenderOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_STRIP;
    mRenderOp.useIndexes = false;

    // Set bounding
    setBoundingBox(Ogre::AxisAlignedBox(-10000000000, -10000000000, -10000000000, 10000000000,
                                        10000000000, 10000000000));
    mRadius = 15000;

    mMaterialPtr = Ogre::MaterialManager::getSingleton().getByName("DeferredShading/AmbientLight");
    assert(mMaterialPtr.isNull() == false);
    mMaterialPtr->load();
}

AmbientLight::~AmbientLight() {
    // need to release IndexData and vertexData created for renderable
    delete mRenderOp.indexData;
    delete mRenderOp.vertexData;
}

Ogre::Real AmbientLight::getBoundingRadius() const {
    return mRadius;
}

Ogre::Real AmbientLight::getSquaredViewDepth(const Ogre::Camera*) const {
    return 0.0;
}

const Ogre::MaterialPtr& AmbientLight::getMaterial() const {
    return mMaterialPtr;
}

void AmbientLight::getWorldTransforms(Ogre::Matrix4* xform) const {
    *xform = Ogre::Matrix4::IDENTITY;
}

void AmbientLight::updateFromCamera(Ogre::Camera* camera) {
    Ogre::Technique* tech = getMaterial()->getBestTechnique();
    Ogre::Vector3 farCorner = camera->getViewMatrix(true) * camera->getWorldSpaceCorners()[4];

    for (unsigned short i = 0; i < tech->getNumPasses(); i++) {
        Ogre::Pass* pass = tech->getPass(i);

        // Get the vertex shader parameters
        Ogre::GpuProgramParametersSharedPtr params = pass->getVertexProgramParameters();

        // Set the camera's far-top-right corner
        if (params->_findNamedConstantDefinition("farCorner"))
            params->setNamedConstant("farCorner", farCorner);

        params = pass->getFragmentProgramParameters();
        if (params->_findNamedConstantDefinition("farCorner"))
            params->setNamedConstant("farCorner", farCorner);
    }
}
}
