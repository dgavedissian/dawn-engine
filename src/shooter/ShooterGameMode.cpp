/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "ShooterGameMode.h"
#include "core/GameSession.h"
#include "renderer/CCamera.h"
#include "renderer/SceneGraph.h"

ShooterEntityPipeline::ShooterEntityPipeline(Context* ctx, SceneManager* scene_manager,
                                             NetInstance* net, Frame* frame)
    : NetEntityPipeline(ctx), scene_manager_(scene_manager), net_(net), frame_(frame) {
}

Entity* ShooterEntityPipeline::createEntityFromType(EntityId entity_id, EntityType type,
                                                    NetRole role) {
    assert(frame_);
    switch (type) {
        case Hash("Ship"): {
            SharedPtr<Ship> ship =
                makeShared<Ship>(context(), net_, scene_manager_, frame_, entity_id, role);
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

    session_->sceneManager()->createStarSystem();

    // Random thing.
    auto rc = module<ResourceCache>();
    auto material = makeShared<Material>(
        context(), makeShared<Program>(context(), rc->get<VertexShader>("base:space/planet.vs"),
                                       rc->get<FragmentShader>("base:space/planet.fs")));
    material->setTexture(rc->get<Texture>("base:space/planet.jpg"));
    material->setUniform("light_direction", Vec3{0.0f, 0.0f, 1.0f});
    material->setUniform("surface_sampler", 0);
    auto renderable = MeshBuilder(context()).texcoords(true).normals(true).createSphere(4000.0f);
    renderable->setMaterial(material);
    auto planet = session_->sceneGraph()->root().newChild(SystemPosition{12000.0f, 0.0f, -6000.0f},
                                                          Quat::identity);
    planet->data.renderable = renderable;

    // Create a camera.
    auto& camera =
        session_->sceneManager()->createEntity(0, {0.0f, 0.0f, 50.0f}, Quat::identity, *frame_);
    camera.addComponent<CCamera>(0.1f, 1000000.0f, 60.0f, 1280.0f / 800.0f);
    camera_controller_ = makeShared<ShipCameraController>(context(), Vec3{0.0f, 15.0f, 50.0f});
    camera_controller_->possess(&camera);

    // If we're running a local-only game, spawn a player.
    /*
    if (!module<Networking>()->isConnected()) {
        auto new_ship = makeShared<Ship>(context(), frame_,
                                         module<SceneManager>()->reserveEntityId(), NetRole::None);
        camera_controller_->follow(new_ship->entity());
        entity_pipeline_->ship_list_.emplace_back(new_ship);
    }
    */

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
