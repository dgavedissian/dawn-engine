/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "renderer/Renderer.h"
#include "renderer/CustomRenderable.h"
#include "scene/space/StarSystem.h"
#include "scene/space/Planet.h"
#include "scene/space/PlanetRings.h"
#include "renderer/Node.h"
#include "resource/ResourceCache.h"

namespace dw {
Planet::Planet(Context* ctx, SystemNode& system_node, StarSystem& star_system,
               const PlanetDesc& desc)
    : SystemBody(ctx, desc.radius, system_node),
      star_system_(star_system),
      desc_(desc),
      axial_tilt_(Vec3(0.0f, 0.0f, 1.0f), -desc_.axial_tilt),
      sun_direction_(0.0f, 0.0f, 1.0f),
      atmosphere_node_(nullptr) {
    auto* rc = module<ResourceCache>();

    auto planet_vs = rc->getUnchecked<VertexShader>("base:space/planet.vs");
    auto planet_fs = rc->getUnchecked<FragmentShader>("base:space/planet.fs");

    // Set up surface material.
    surface_material_ =
        makeShared<Material>(context(), makeShared<Program>(context(), planet_vs, planet_fs));
    surface_material_->setTexture(rc->getUnchecked<Texture>(desc.surface_texture), 0);
    surface_material_->setTexture(rc->getUnchecked<Texture>(desc.normal_map_texture), 1);
    surface_material_->setUniform("surface_map", 0);
    surface_material_->setUniform("normal_map", 1);
    surface_material_->setUniform("sun_direction", Vec3{0.0f, 0.0f, 1.0f});

    // Create surface.
    auto surface_renderable = CustomRenderable::Builder(context())
                                  .texcoords(true)
                                  .normals(true)
                                  .tangents(true)
                                  .createSphere(desc.radius, 48, 48);
    surface_renderable->setMaterial(surface_material_);
    system_node.data.renderable = surface_renderable;

    // Create the atmosphere.
    if (desc.has_atmosphere) {
        auto atmosphere_vs = rc->getUnchecked<VertexShader>("base:materials/scene/atmosphere.vs");
        auto atmosphere_fs = rc->getUnchecked<FragmentShader>("base:materials/scene/atmosphere.fs");

        // Set up atmosphere material.
        atmosphere_material_ = makeShared<Material>(
            context(), makeShared<Program>(context(), atmosphere_vs, atmosphere_fs));
        atmosphere_material_->setCullFrontFace(gfx::CullFrontFace::CW);
        atmosphere_material_->setDepthWrite(false);
        atmosphere_material_->setStateEnable(gfx::RenderState::Blending);
        atmosphere_material_->enableAlphaBlending();

        // Create atmosphere renderable.
        auto atmosphere_renderable =
            CustomRenderable::Builder(context()).createSphere(desc.atmosphere.radius, 128, 128);
        atmosphere_renderable->setMaterial(atmosphere_material_);
        atmosphere_node_ = system_node.newChild();
        atmosphere_node_->data.renderable = atmosphere_renderable;

        // Compute and set shader constants
        Vec3 wavelength(0.65f, 0.57f, 0.475f);  // 650nm for red, 570mn for green, 475nm for blue
        Vec3 inv_wavelength(1.0f / pow(wavelength.x, 4.0f), 1.0f / pow(wavelength.y, 4.0f),
                            1.0f / pow(wavelength.z, 4.0f));
        float Kr = 0.0025f;
        float Km = 0.0015f;
        float ESun = 15.0f;
        float scale = 1.0f / (desc.atmosphere.radius - desc.radius);
        float scale_depth = 0.25f;
        float g = -0.95f;

        surface_material_->setUniform("inv_wavelength",
                                      Vec3(inv_wavelength.x, inv_wavelength.y, inv_wavelength.z));
        surface_material_->setUniform(
            "radius", Vec4(desc.radius, desc.radius * desc.radius, desc.atmosphere.radius,
                           desc.atmosphere.radius * desc.atmosphere.radius));
        surface_material_->setUniform(
            "kr_km", Vec4(Kr * ESun, Kr * 4.0f * math::pi, Km * ESun, Km * 4.0f * math::pi));
        surface_material_->setUniform("atmos_scale", Vec3(scale, scale_depth, scale / scale_depth));

        atmosphere_material_->setUniform(
            "inv_wavelength", Vec3(inv_wavelength.x, inv_wavelength.y, inv_wavelength.z));
        atmosphere_material_->setUniform(
            "radius", Vec4(desc.radius, desc.radius * desc.radius, desc.atmosphere.radius,
                           desc.atmosphere.radius * desc.atmosphere.radius));
        atmosphere_material_->setUniform(
            "kr_km", Vec4(Kr * ESun, Kr * 4.0f * math::pi, Km * ESun, Km * 4.0f * math::pi));
        atmosphere_material_->setUniform("atmos_scale",
                                         Vec3(scale, scale_depth, scale / scale_depth));
        atmosphere_material_->setUniform("g", Vec2(g, g * g));
    }

    // Create the rings.
    if (desc.has_rings) {
        rings_ = makeUnique<PlanetRings>(ctx, desc_, *system_node.newChild());
    }

    system_node.orientation = axial_tilt_;
    if (rings_) {
        rings_->ring_system_node_.orientation = axial_tilt_;
    }
}

void Planet::preRender(Frame& frame) {
    SystemBody::preRender(frame);

    // Update rings parameters.
    if (desc_.has_rings) {
        rings_->updatePositions(system_node_.position.getRelativeTo(frame.position()),
                                sun_direction_);
    }
}

void Planet::update(float dt, Frame& frame, const Vec3& camera_position) {
    SystemBody::update(dt, frame, camera_position);

    // Updates based on camera position
    //===========================================================================
    Vec3 local_camera_position = (frame.position() + camera_position).getRelativeTo(system_node_.position);

    // Update atmosphere shader
    if (desc_.has_atmosphere) {
        float camera_height = local_camera_position.Length();
        surface_material_->setUniform("camera_position", local_camera_position);
        surface_material_->setUniform("camera_height",
                                      Vec2(camera_height, camera_height * camera_height));
        atmosphere_material_->setUniform("camera_position", local_camera_position);
        atmosphere_material_->setUniform("camera_height",
                                         Vec2(camera_height, camera_height * camera_height));
    }

    // Update rings
    if (desc_.has_rings) {
        rings_->update(camera_position);
    }
}

void Planet::updatePosition(double time) {
    // TODO Rotating the planet causes issues with the atmosphere shader
    // mSurfaceNode->setOrientation(axial_tilt_ * Quat(Vec3::unitY, time / mDesc.rotational_period *
    // 2.0f * math::pi));
    SystemBody::updatePosition(time);

    if (atmosphere_node_) {
        atmosphere_node_->position = system_node_.position;
    }
    if (rings_) {
        rings_->ring_system_node_.position = system_node_.position;
    }

    // Detect a star object.
    // TODO support multiple stars
    if (!star_system_.getStars().empty()) {
        SystemPosition star_position = star_system_.getStars().front()->getSystemNode().position;
        sun_direction_ = -system_node_.position.getRelativeTo(star_position).Normalized();
        Vec3 local_sun_direction = axial_tilt_.Inverted() * sun_direction_;

        // Update surface parameters.
        surface_material_->setUniform("sun_direction", sun_direction_);

        // Update atmosphere parameters.
        if (desc_.has_atmosphere) {
            atmosphere_material_->setUniform("sun_direction", sun_direction_);
        }
    }
}
}  // namespace dw
