/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "ShooterGameMode.h"

ShooterEntityPipeline::ShooterEntityPipeline(Context* ctx) : NetEntityPipeline(ctx) {
}

Entity* ShooterEntityPipeline::createEntityFromType(EntityId entity_id, EntityType type, NetRole role) {
  switch (type) {
    case Hash("Ship"): {
      SharedPtr<Ship> ship = makeShared<Ship>(context(), entity_id, role);
      Entity* entity = ship->entity();
      entity->component<ShipControls>()->ship = ship;
      ship_list_.emplace_back(std::move(ship));
      return entity;
    }
    default:
      return nullptr;
  }
}

ShooterGameMode::ShooterGameMode(Context* ctx, ShooterEntityPipeline* entity_pipeline)
: NetGameMode(ctx), entity_pipeline_(entity_pipeline) {
}

void ShooterGameMode::clientOnJoinServer() {
  log().info("Client: connected to server.");
  module<Networking>()->sendSpawnRequest(
      Hash("Ship"),
      [this](Entity& entity) {
        log().info("Received spawn response. Triggering callback.");
        camera_controller->follow(&entity);
      },
      /* messaging_proxy */ true);
}

void ShooterGameMode::serverOnStart() {
  // ship = makeShared<Ship>(context());
  // camera_controller->follow(ship->entity());
}

void ShooterGameMode::serverOnClientConnected() {
  log().info("Server: Client connected.");
}

void ShooterGameMode::serverOnClientDisconnected() {
  log().info("Server: Client disconnected.");
}

void ShooterGameMode::onStart() {
  NetGameMode::onStart();

  module<SceneManager>()->createStarSystem();

  // Random thing.
  auto rc = module<ResourceCache>();
  auto material = makeShared<Material>(
      context(), makeShared<Program>(context(), rc->get<VertexShader>("base:space/planet.vs"),
                                     rc->get<FragmentShader>("base:space/planet.fs")));
  material->setTexture(rc->get<Texture>("base:space/planet.jpg"));
  material->setUniform("light_direction", Vec3{0.0f, 0.0f, 1.0f});
  material->setUniform("surface_sampler", 0);
  auto renderable =
      MeshBuilder(context()).texcoords(true).normals(true).createSphere(1000.0f);
  renderable->setMaterial(material);
  module<SceneManager>()
      ->createEntity(0, Position{4000.0f, 0.0f, 0.0f}, Quat::identity)
      .addComponent<RenderableComponent>(renderable);

  // Create a camera.
  auto& camera = module<SceneManager>()
      ->createEntity(0, Position{0.0f, 0.0f, 50.0f}, Quat::identity)
      .addComponent<Camera>(0.1f, 100000.0f, 60.0f, 1280.0f / 800.0f);
  camera_controller = makeShared<ShipCameraController>(context(), Vec3{0.0f, 15.0f, 50.0f});
  camera_controller->possess(&camera);
}

void ShooterGameMode::onEnd() {
  NetGameMode::onEnd();
}

void ShooterGameMode::update(float dt) {
  NetGameMode::update(dt);
  for (auto& ship : entity_pipeline_->ship_list_) {
    ship->update(dt);
  }
  camera_controller->update(dt);
}