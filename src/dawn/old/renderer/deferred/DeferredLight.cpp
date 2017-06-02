/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "LightMaterialGenerator.h"
#include "DeferredLight.h"
#include "DeferredLightCompPass.h"

#define ENABLE_BIT(mask, flag) (mask) |= (flag)
#define DISABLE_BIT(mask, flag) (mask) &= ~(flag)

namespace dw {

DeferredLight::DeferredLight(MaterialGenerator* sys, Ogre::Light* parentLight,
                             DeferredLightRenderOperation* renderOp)
    : mLightRenderOp(renderOp),
      mParentLight(parentLight),
      mIgnoreWorldTrans(false),
      mRadius(0.0f),
      mHeight(0.0f),
      mGenerator(sys),
      mPermutation(0) {
    updateFromParent();
}

DeferredLight::~DeferredLight() {
}

void DeferredLight::setAttenuation(float c, float b, float a) {
    float outerRadius = mParentLight->getAttenuationRange();

    // There is attenuation? Set material accordingly
    if (c != 1.0f || b != 0.0f || a != 0.0f) {
        ENABLE_BIT(mPermutation, LightMaterialGenerator::MI_ATTENUATED);

        if (mParentLight->getType() == Ogre::Light::LT_POINT) {
            // Calculate radius from attenuation
            int thresholdLevel = 10;  // difference of 10-15 levels deemed unnoticeable
            float threshold = 1.0f / ((float)thresholdLevel / 256.0f);

            // Use quadratic formula to Determine outer radius
            c = c - threshold;
            float d = sqrt(b * b - 4.0f * a * c);
            outerRadius = (-2.0f * c) / (b + d);
            outerRadius *= 1.2f;
        }
    } else {
        DISABLE_BIT(mPermutation, LightMaterialGenerator::MI_ATTENUATED);
    }

    setupGeometry(outerRadius);
}

void DeferredLight::setSpecularColour(const Ogre::ColourValue& col) {
    // setCustomParameter(2, Vector4(col.r, col.g, col.b, col.a));

    // There is a specular component? Set material accordingly
    if (col.r != 0.0f || col.g != 0.0f || col.b != 0.0f) {
        ENABLE_BIT(mPermutation, LightMaterialGenerator::MI_SPECULAR);
    } else {
        DISABLE_BIT(mPermutation, LightMaterialGenerator::MI_SPECULAR);
    }
}

void DeferredLight::setupGeometry(float radius) {
    // Disable all 3 bits
    DISABLE_BIT(mPermutation, LightMaterialGenerator::MI_POINT);
    DISABLE_BIT(mPermutation, LightMaterialGenerator::MI_SPOTLIGHT);
    DISABLE_BIT(mPermutation, LightMaterialGenerator::MI_DIRECTIONAL);

    mIgnoreWorldTrans = mParentLight->getType() == Ogre::Light::LT_DIRECTIONAL;

    switch (mParentLight->getType()) {
        case Ogre::Light::LT_DIRECTIONAL:
            // TODO: Effectively infinite bounds
            mRenderOp = mLightRenderOp->mDirectionalLightOp;
            setBoundingBox(Ogre::AxisAlignedBox(-10000, -10000, -10000, 10000, 10000, 10000));
            mRadius = 15000.0f;
            ENABLE_BIT(mPermutation, LightMaterialGenerator::MI_DIRECTIONAL);
            break;

        case Ogre::Light::LT_POINT:
            mRenderOp = mLightRenderOp->mPointLightOp;
            setBoundingBox(Ogre::AxisAlignedBox(Ogre::Vector3(-radius, -radius, -radius),
                                                Ogre::Vector3(radius, radius, radius)));
            mRadius = radius;
            ENABLE_BIT(mPermutation, LightMaterialGenerator::MI_POINT);
            break;

        case Ogre::Light::LT_SPOTLIGHT:
            mHeight = mParentLight->getAttenuationRange();
            mRadius =
                math::Tan(mParentLight->getSpotlightOuterAngle().valueRadians() * 0.5f) * mHeight;
            mRenderOp = mLightRenderOp->mSpotlightOp;
            setBoundingBox(Ogre::AxisAlignedBox(Ogre::Vector3(-mRadius, 0, -mRadius),
                                                Ogre::Vector3(mRadius, mHeight, mRadius)));
            ENABLE_BIT(mPermutation, LightMaterialGenerator::MI_SPOTLIGHT);
            break;

        default:
            break;
    }
}

Ogre::Real DeferredLight::getBoundingRadius() const {
    return std::max(mRadius, mHeight);
}

Ogre::Real DeferredLight::getSquaredViewDepth(const Ogre::Camera* cam) const {
    if (mIgnoreWorldTrans) {
        return 0.0f;
    } else {
        Ogre::Vector3 dist =
            cam->getDerivedPosition() - getParentSceneNode()->_getDerivedPosition();
        return dist.squaredLength();
    }
}

const Ogre::MaterialPtr& DeferredLight::getMaterial() const {
    return mGenerator->GetMaterial(mPermutation);
}

void DeferredLight::getWorldTransforms(Ogre::Matrix4* xform) const {
    switch (mParentLight->getType()) {
        case Ogre::Light::LT_SPOTLIGHT: {
            Ogre::Quaternion quat =
                Ogre::Vector3::UNIT_Y.getRotationTo(mParentLight->getDerivedDirection());
            xform->makeTransform(mParentLight->getDerivedPosition(),
                                 Ogre::Vector3(mRadius, mHeight, mRadius), quat);
        } break;

        case Ogre::Light::LT_POINT:
            xform->makeTransform(mParentLight->getDerivedPosition(), Ogre::Vector3(mRadius),
                                 Ogre::Quaternion::IDENTITY);
            break;

        case Ogre::Light::LT_DIRECTIONAL:
            xform->makeTransform(mParentLight->getDerivedPosition(), Ogre::Vector3::UNIT_SCALE,
                                 Ogre::Quaternion::IDENTITY);
            break;
    }
}

void DeferredLight::updateFromParent() {
    // TODO : Don't do this unless something changed
    setAttenuation(mParentLight->getAttenuationConstant(), mParentLight->getAttenuationLinear(),
                   mParentLight->getAttenuationQuadric());
    setSpecularColour(mParentLight->getSpecularColour());

    if (getCastChadows()) {
        ENABLE_BIT(mPermutation, LightMaterialGenerator::MI_SHADOW_CASTER);
    } else {
        DISABLE_BIT(mPermutation, LightMaterialGenerator::MI_SHADOW_CASTER);
    }
}

bool DeferredLight::isCameraInside(Ogre::Camera* camera) {
    switch (mParentLight->getType()) {
        case Ogre::Light::LT_DIRECTIONAL:
            return false;

        case Ogre::Light::LT_POINT: {
            Ogre::Real distanceFromLight =
                camera->getDerivedPosition().distance(mParentLight->getDerivedPosition());

            // Small epsilon fix to account for the fact that we aren't a true sphere.
            return distanceFromLight <= mRadius + camera->getNearClipDistance() + 0.1f;
        }

        case Ogre::Light::LT_SPOTLIGHT: {
            Ogre::Vector3 lightPos = mParentLight->getDerivedPosition();
            Ogre::Vector3 lightDir = mParentLight->getDerivedDirection();
            Ogre::Radian attAngle = mParentLight->getSpotlightOuterAngle();

            // Extend the analytic cone's radius by the near clip range by moving its
            // tip accordingly.
            // Some trigonometry needed here.
            Ogre::Vector3 clipRangeFix =
                -lightDir * (camera->getNearClipDistance() / Ogre::Math::Tan(attAngle / 2));
            lightPos += clipRangeFix;

            Ogre::Vector3 lightToCamDir = camera->getDerivedPosition() - lightPos;
            Ogre::Real distanceFromLight = lightToCamDir.normalise();

            Ogre::Real cosAngle = lightToCamDir.dotProduct(lightDir);
            Ogre::Radian angle = Ogre::Math::ACos(cosAngle);

            // Check whether we will see the cone from our current POV.
            return (distanceFromLight <=
                    (mParentLight->getAttenuationRange() / cosAngle + clipRangeFix.length())) &&
                   (angle <= attAngle);
        }

        default:
            return false;
    }
}

bool DeferredLight::getCastChadows() const {
    return mParentLight->_getManager()->isShadowTechniqueInUse() &&
           mParentLight->getCastShadows() &&
           (mParentLight->getType() == Ogre::Light::LT_DIRECTIONAL ||
            mParentLight->getType() == Ogre::Light::LT_SPOTLIGHT);
}

void DeferredLight::updateFromCamera(Ogre::Camera* camera) {
    // Set shader params
    const Ogre::MaterialPtr& mat = getMaterial();

    if (!mat->isLoaded())
        mat->load();

    Ogre::Technique* tech = mat->getBestTechnique();
    Ogre::Vector3 farCorner = camera->getViewMatrix(true) * camera->getWorldSpaceCorners()[4];

    for (unsigned short i = 0; i < tech->getNumPasses(); i++) {
        Ogre::Pass* pass = tech->getPass(i);

        // get the vertex shader parameters
        // set the camera's far-top-right corner
        Ogre::GpuProgramParametersSharedPtr params = pass->getVertexProgramParameters();
        if (params->_findNamedConstantDefinition("farCorner"))
            params->setNamedConstant("farCorner", farCorner);
        params = pass->getFragmentProgramParameters();
        if (params->_findNamedConstantDefinition("farCorner"))
            params->setNamedConstant("farCorner", farCorner);

        // If inside light geometry, render back faces with CMPF_GREATER, otherwise normally
        if (mParentLight->getType() == Ogre::Light::LT_DIRECTIONAL) {
            pass->setCullingMode(Ogre::CULL_CLOCKWISE);
            pass->setDepthCheckEnabled(false);
        } else {
            pass->setDepthCheckEnabled(true);

            if (isCameraInside(camera)) {
                pass->setCullingMode(Ogre::CULL_ANTICLOCKWISE);
                pass->setDepthFunction(Ogre::CMPF_GREATER_EQUAL);
            } else {
                pass->setCullingMode(Ogre::CULL_CLOCKWISE);
                pass->setDepthFunction(Ogre::CMPF_LESS_EQUAL);
            }
        }

        Ogre::Camera shadowCam("ShadowCameraSetupCam", nullptr);
        shadowCam._notifyViewport(camera->getViewport());
        Ogre::SceneManager* sm = mParentLight->_getManager();
        sm->getShadowCameraSetup()->getShadowCamera(sm, camera, camera->getViewport(), mParentLight,
                                                    &shadowCam, 0);

        // Get the shadow camera position
        if (params->_findNamedConstantDefinition("shadowCamPos"))
            params->setNamedConstant("shadowCamPos", shadowCam.getPosition());
        if (params->_findNamedConstantDefinition("shadowFarClip"))
            params->setNamedConstant("shadowFarClip", shadowCam.getFarClipDistance());
    }
}
}  // namespace dw
