/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "net/NetSystem.h"
#include "net/NetGameMode.h"

namespace dw {
NetGameMode::NetGameMode(Context* ctx) : GameMode(ctx) {
}

NetGameMode::~NetGameMode() {
}

void NetGameMode::onJoinServer() {
}

void NetGameMode::onClientConnected() {
}

void NetGameMode::onClientDisconnected() {
}

void NetGameMode::onStart() {
    addEventListener<JoinServerEvent>(makeEventDelegate(this, &NetGameMode::eventOnJoinServer));
    addEventListener<ServerClientConnectedEvent>(
        makeEventDelegate(this, &NetGameMode::eventOnServerClientConnected));
    addEventListener<ServerClientDisconnectedEvent>(
        makeEventDelegate(this, &NetGameMode::eventOnServerClientDisconnected));
}

void NetGameMode::onEnd() {
    removeEventListener<JoinServerEvent>(makeEventDelegate(this, &NetGameMode::eventOnJoinServer));
    removeEventListener<ServerClientConnectedEvent>(
        makeEventDelegate(this, &NetGameMode::eventOnServerClientConnected));
    removeEventListener<ServerClientDisconnectedEvent>(
        makeEventDelegate(this, &NetGameMode::eventOnServerClientDisconnected));
}

void NetGameMode::update(float dt) {
}

void NetGameMode::eventOnJoinServer(const JoinServerEvent&) {
    onJoinServer();
}

void NetGameMode::eventOnServerClientConnected(const ServerClientConnectedEvent& e) {
    onClientConnected();
}

void NetGameMode::eventOnServerClientDisconnected(const ServerClientDisconnectedEvent& e) {
    onClientDisconnected();
}
}  // namespace dw
