/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "ShooterGameMode.h"
#include "core/GameSession.h"
#include "renderer/CCamera.h"
#include "renderer/SceneGraph.h"
#include "resource/ResourceCache.h"
#include "renderer/CustomRenderable.h"

ShooterEntityPipeline::ShooterEntityPipeline(Context* ctx, SceneManager* scene_manager,
                                             NetInstance* net, Frame* frame)
    : NetEntityPipeline(ctx), scene_manager_(scene_manager), net_(net), frame_(frame) {
}

Entity* ShooterEntityPipeline::createEntityFromType(EntityType type, NetRole role) {
    assert(frame_);
    switch (type) {
        case Hash("Ship"): {
            SharedPtr<Ship> ship = makeShared<Ship>(context(), net_, scene_manager_, frame_, role);
            Entity* entity = ship->entity();
            entity->component<CShipControls>()->ship = ship;
            ship_list_.emplace_back(std::move(ship));
            return entity;
        }
        case Hash("Projectile"):
            return scene_manager_->system<SProjectile>()->createNewProjectile(
                0, Vec3::zero, Vec3::unitZ, Vec3::zero, Colour{});
        default:
            return nullptr;
    }
}

ShooterGameMode::ShooterGameMode(Context* ctx, GameSession* session, Frame* frame,
                                 SharedPtr<ShooterEntityPipeline> entity_pipeline)
    : NetGameMode(ctx, session), frame_(frame), entity_pipeline_(entity_pipeline) {
}

void ShooterGameMode::clientOnJoinServer() {
    log().info("Client: connected to server.");
    session_->net()->sendSpawnRequest(
        Hash("Ship"),
        [this](Entity& entity) {
            log().info("Received spawn response. Triggering callback.");
            camera_controller_->follow(&entity);
        },
        /* authoritative_proxy */ true);
}

void ShooterGameMode::serverOnStart() {
}

void ShooterGameMode::serverOnClientConnected() {
    log().info("Server: Client connected.");
}

void ShooterGameMode::serverOnClientDisconnected() {
    log().info("Server: Client disconnected.");
}

void ShooterGameMode::onStart() {
    NetGameMode::onStart();

    // Set up the environment.
    auto& star_system = session_->sceneManager()->createStarSystem();

    auto& star = star_system.addStar(StarDesc{695510.0f, SpectralClass::G}, star_system.root(),
                                     makeUnique<CircularOrbit>(0.0f, 1.0f));

    PlanetDesc planet_desc;
    planet_desc.radius = 6371.0f;
    planet_desc.axial_tilt = 0.2f;
    planet_desc.surface_texture = "base:space/planet2.jpg";
    planet_desc.normal_map_texture = "base:space/planet2_normal.jpg";
    planet_desc.has_atmosphere = true;
    planet_desc.rings.min_radius = planet_desc.radius + 700.0f;
    planet_desc.rings.max_radius = planet_desc.radius + 8000.0f;
    auto& planet = star_system.addPlanet(planet_desc, star,
                                         makeUnique<CircularOrbit>(147.11f * 1e6f, 20000.0f));
    star_system.updatePosition(14000.0);
    // TODO: Move this to the scene manager.
    session_->sceneGraph()->preRenderCameraCallback =
        [&star_system, this](float dt, const detail::Transform& camera_transform,
                             const Mat4& view_matrix, const Mat4& proj_matrix) {
            star_system.update(dt, *frame_, camera_transform.position, view_matrix, proj_matrix);
        };
    frame_->position() =
        planet.getSystemNode().position + Vec3(0.0f, 0.0f, planet_desc.radius * 2.0f);

    // Create a camera.
    auto& camera =
        session_->sceneManager()->createEntity(0, {0.0f, 0.0f, 50.0f}, Quat::identity, *frame_);
    camera.addComponent<CCamera>(0.1f, 1000000.0f, 60.0f, 1280.0f / 800.0f);
    camera_controller_ = makeShared<ShipCameraController>(context(), Vec3{0.0f, 15.0f, 50.0f});
    camera_controller_->possess(&camera);

    // If we're running a local-only game, spawn a player.
    if (!session_->net()) {
        auto new_ship = makeShared<Ship>(context(), session_->net(), session_->sceneManager(),
                                         frame_, NetRole::None);
        camera_controller_->follow(new_ship->entity());
        entity_pipeline_->ship_list_.emplace_back(new_ship);
    }

    // Spawn a test ship.
    /*
    auto test_ship = makeShared<Ship>(context());
    auto rb = test_ship->entity()->component<CRigidBody>()->_rigidBody();
    btTransform xform = rb->getWorldTransform();
    xform.setOrigin({0.0, 60.0, 0.0});
    rb->setWorldTransform(xform);
    // test_ship->entity()->transform()->position() = Position{0.0, 0.0, 200.0};
    entity_pipeline_->ship_list_.emplace_back(test_ship);
    */
}

void ShooterGameMode::onEnd() {
    NetGameMode::onEnd();
}

void ShooterGameMode::update(float dt) {
    NetGameMode::update(dt);
    for (auto& ship : entity_pipeline_->ship_list_) {
        ship->update(dt);
    }
    camera_controller_->update(dt);
}
