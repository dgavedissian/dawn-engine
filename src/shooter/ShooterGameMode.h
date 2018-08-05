/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "DawnEngine.h"
#include "renderer/MeshBuilder.h"
#include "renderer/Mesh.h"

#include "ShipCameraController.h"
#include "Ship.h"

#include "CProjectile.h"

#include "net/NetEntityPipeline.h"
#include "net/NetGameMode.h"

using namespace dw;

class ShooterEntityPipeline : public NetEntityPipeline {
public:
    DW_OBJECT(ShooterEntityPipeline);

    Vector<SharedPtr<Ship>> ship_list_;

    explicit ShooterEntityPipeline(Context* ctx, Frame* frame);
    ~ShooterEntityPipeline() override = default;

    Entity* createEntityFromType(EntityId entity_id, EntityType type, NetRole role) override;

private:
    Frame* frame_;
};

class ShooterGameMode : public NetGameMode {
public:
    DW_OBJECT(ShooterGameMode);

    ShooterGameMode(Context* ctx, Frame* frame, ShooterEntityPipeline* entity_pipeline);

    // NetGameMode
    void clientOnJoinServer() override;
    void serverOnStart() override;
    void serverOnClientConnected() override;
    void serverOnClientDisconnected() override;

    // GameMode
    void onStart() override;
    void onEnd() override;
    void update(float dt) override;

private:
    Frame* frame_;
    ShooterEntityPipeline* entity_pipeline_;
    SharedPtr<ShipCameraController> camera_controller_;
};
