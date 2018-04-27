/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "DawnEngine.h"
#include "renderer/MeshBuilder.h"
#include "renderer/Mesh.h"

#include "Ship.h"

#include "net/NetEntityPipeline.h"
#include "net/NetGameMode.h"

using namespace dw;

class ShooterEntityPipeline : public NetEntityPipeline {
public:
  DW_OBJECT(ShooterEntityPipeline);

  Vector<SharedPtr<Ship>> ship_list_;

  explicit ShooterEntityPipeline(Context* ctx);
  ~ShooterEntityPipeline() override = default;

  Entity* createEntityFromType(EntityId entity_id, EntityType type, NetRole role) override;
};

class ShooterGameMode : public NetGameMode {
public:
  DW_OBJECT(ShooterGameMode);

  ShooterGameMode(Context* ctx, ShooterEntityPipeline* entity_pipeline);

  // NetGameMode
  void clientOnJoinServer() override ;
  void serverOnStart() override;
  void serverOnClientConnected() override;
  void serverOnClientDisconnected() override;

  // GameMode
  void onStart() override;
  void onEnd() override;
  void update(float dt) override;

private:
  ShooterEntityPipeline* entity_pipeline_;
  SharedPtr<ShipCameraController> camera_controller;
};