/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "renderer/Renderer.h"
#include "renderer/CustomRenderable.h"
#include "scene/space/StarSystem.h"
#include "scene/space/Planet.h"
#include "scene/space/Star.h"
#include "resource/ResourceCache.h"

namespace dw {
Planet::Planet(Context* ctx, SystemNode& system_node, StarSystem& star_system,
               const PlanetDesc& desc)
    : SystemBody(ctx, desc.radius, system_node),
      star_system_(star_system),
      desc_(desc),
      axial_tilt_(Vec3(0.0f, 0.0f, 1.0f), -desc_.axial_tilt) {
    auto* rc = module<ResourceCache>();

    auto planet_vs = *rc->get<VertexShader>("base:space/planet.vs");
    auto planet_fs = *rc->get<FragmentShader>("base:space/planet.fs");

    // Set up surface material.
    surface_material_ =
        makeShared<Material>(context(), makeShared<Program>(context(), planet_vs, planet_fs));
    surface_material_->setTexture(*rc->get<Texture>(desc.surface_texture));
    surface_material_->setUniform("surface_sampler", 0);
    surface_material_->setUniform("light_direction", Vec3{0.0f, 0.0f, 1.0f});

    // Create surface.
    auto surface_renderable =
        CustomRenderable::Builder(context()).texcoords(true).normals(true).createSphere(desc.radius,
                                                                                        48, 48);
    surface_renderable->setMaterial(surface_material_);
    system_node.data.renderable = surface_renderable;

    /*
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
    pass->getTextureUnitState("surface")->setTextureName(desc.surface_texture);
    pass->getTextureUnitState("surface")->setHardwareGammaEnabled(true);
    pass->getTextureUnitState("night")->setTextureName(desc.night_texture);
    pass->getTextureUnitState("night")->setHardwareGammaEnabled(true);

    // Create the atmosphere
    if (desc.has_atmosphere) {
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
    if (desc.has_rings) {
        mRingSystem.reset(new Rings(desc, this));

        // Set ring system properties on the surface shader
        Ogre::Vector4 ringDimensions(desc.rings.min_radius, desc.rings.max_radius,
                                     desc.rings.max_radius - desc.rings.min_radius, 0.0f);
        Ogre::Pass* surfacePass = mSurfaceMaterial->getTechnique(0)->getPass(0);
        surfacePass->getTextureUnitState("rings")->setTextureName(desc.rings.texture);
        surfacePass->getTextureUnitState("rings")->setHardwareGammaEnabled(true);
        mSurfaceSubEntity->setCustomParameter(4, ringDimensions);
    }

    // Configure planet shader
    mSurfaceSubEntity->setCustomParameter(
        0, Ogre::Vector4(desc.night_texture.length() > 0 ? 1.0f : 0.0f, desc.has_atmosphere,
                         desc.has_rings, 0.0f));
                         */
}

Planet::~Planet() {
    /*
    mRingSystem.reset();

    if (mAtmosphereEntity) {
        mRenderSystem->getSceneMgr()->destroySceneNode(mAtmosphereNode);
        mRenderSystem->getSceneMgr()->destroyEntity(mAtmosphereEntity);
        mAtmosphereMaterial.setNull();
        mAtmosphereMesh.setNull();
    }

    mRenderSystem->getSceneMgr()->destroySceneNode(mSurfaceNode);
    mRenderSystem->getSceneMgr()->destroyEntity(mSurfaceEntity);
     */
}

void Planet::preRender() {
    /*
    Vec3 cameraSpacePosition = mPosition.toCameraSpace(camera);

    // Scale the surface and atmosphere
    mSurfaceNode->setPosition(cameraSpacePosition);
    if (mDesc.has_atmosphere)
        mAtmosphereNode->setPosition(mSurfaceNode->getPosition());

    // Detect a sun object
    // TODO support multiple stars
    if (star_system_->getStars().size() > 0) {
        Position sunPosition = (*star_system_->getStars().begin())->getPosition();
        Vec3 sunDirection = -mPosition.getRelativeTo(sunPosition).Normalized();
        Vec3 localSunDirection = axial_tilt_.Inverted() * sunDirection;

        // Set the direction in the planet shader
        mSurfaceSubEntity->setCustomParameter(1, Ogre::Vector4(mSurfaceNode->getPosition()));
        mSurfaceSubEntity->setCustomParameter(2, Ogre::Vector4(sunDirection));
        mSurfaceSubEntity->setCustomParameter(3, Ogre::Vector4(localSunDirection));

        // Set the direction in the atmosphere shader
        if (mDesc.has_atmosphere)
            mAtmosphereEntity->getSubEntity(0)->setCustomParameter(
                4, Ogre::Vector4(sunDirection.x, sunDirection.y, sunDirection.z, 0.0f));

        // Update ring shader parameters
        if (mRingSystem) {
            // Update ring dimensions
            Ogre::Vector4 ringDimensions(
                Ogre::Vector3(mDesc.rings.min_radius, mDesc.rings.max_radius,
                              mDesc.rings.max_radius - mDesc.rings.min_radius));
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
    if (mDesc.has_rings)
        mRingSystem->update(mSurfaceNode->getOrientation().Inverse() * localCameraPosition);

    // Update atmosphere shader
    if (mDesc.has_atmosphere) {
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
     */
}

void Planet::updatePosition(double time) {
    // Detect a star object.
    // TODO support multiple stars
    if (!star_system_.getStars().empty()) {
        SystemPosition star_position = star_system_.getStars().front()->getSystemNode().position;
        Vec3 sun_direction = -system_node_.position.getRelativeTo(star_position).Normalized();
        Vec3 local_sun_direction = axial_tilt_.Inverted() * sun_direction;

        // Update surface shader.
        surface_material_->setUniform("light_direction", sun_direction);
        /*
        mSurfaceSubEntity->setCustomParameter(1, Ogre::Vector4(mSurfaceNode->getPosition()));
        mSurfaceSubEntity->setCustomParameter(2, Ogre::Vector4(sun_direction));
        mSurfaceSubEntity->setCustomParameter(3, Ogre::Vector4(local_sun_direction));
         */

        // Update atmosphere shader.
        /*
        if (mDesc.has_atmosphere)
            mAtmosphereEntity->getSubEntity(0)->setCustomParameter(
                4, Ogre::Vector4(sun_direction.x, sun_direction.y, sun_direction.z, 0.0f));

        // Update ring shader parameters
        if (mRingSystem) {
            // Update ring dimensions
            Ogre::Vector4 ringDimensions(
                Ogre::Vector3(mDesc.rings.min_radius, mDesc.rings.max_radius,
                              mDesc.rings.max_radius - mDesc.rings.min_radius));
            mSurfaceSubEntity->setCustomParameter(4, ringDimensions);

            // Convert light direction into a position by moving it from the planets
            // position along the direction vector by a large number.
            // This is somewhat hacky but it works
            Vec3 star_position = mSurfaceNode->getPosition() + sun_direction * 1e9f;
            Vec3 sunToPlanet = mSurfaceNode->getPosition() - star_position;
            Ogre::SubEntity* se = mRingSystem->mRingEntity->getSubEntity(0);
            se->setCustomParameter(0, Ogre::Vector4(mDesc.radius, 0.0f, 0.0f, 0.0f));
            se->setCustomParameter(1, Ogre::Vector4(ringDimensions));
            se->setCustomParameter(2, Ogre::Vector4(mSurfaceNode->getPosition()));
            se->setCustomParameter(3, Ogre::Vector4(1.0f / (mDesc.atmosphere.radius - mDesc.radius),
                                                    0.0f, 0.0f, 0.0f));
            se->setCustomParameter(5, Ogre::Vector4(sunToPlanet.x, sunToPlanet.y, sunToPlanet.z,
                                                    sunToPlanet.Length()));
            se->setCustomParameter(6, Ogre::Vector4(star_position));
        }
         */
    }

    // TODO Rotating the planet causes issues with the atmosphere shader
    // mSurfaceNode->setOrientation(axial_tilt_ * Quat(Vec3::unitY, time / mDesc.rotational_period *
    // 2.0f * math::pi));
    SystemBody::updatePosition(time);
}
}  // namespace dw
