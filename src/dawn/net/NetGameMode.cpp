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

void NetGameMode::clientOnJoinServer() {
}

void NetGameMode::serverOnStart() {
}

void NetGameMode::serverOnEnd() {
}

void NetGameMode::serverOnClientConnected() {
}

void NetGameMode::serverOnClientDisconnected() {
}

void NetGameMode::onStart() {
    addEventListener<JoinServerEvent>(makeEventDelegate(this, &NetGameMode::eventOnJoinServer));
    addEventListener<ServerClientConnectedEvent>(
        makeEventDelegate(this, &NetGameMode::eventOnServerClientConnected));
    addEventListener<ServerClientDisconnectedEvent>(
        makeEventDelegate(this, &NetGameMode::eventOnServerClientDisconnected));

    if (subsystem<NetSystem>()->isServer()) {
        serverOnStart();
        server_started_ = true;
    }
}

void NetGameMode::onEnd() {
    removeEventListener<JoinServerEvent>(makeEventDelegate(this, &NetGameMode::eventOnJoinServer));
    removeEventListener<ServerClientConnectedEvent>(
        makeEventDelegate(this, &NetGameMode::eventOnServerClientConnected));
    removeEventListener<ServerClientDisconnectedEvent>(
        makeEventDelegate(this, &NetGameMode::eventOnServerClientDisconnected));

    if (server_started_) {
        serverOnEnd();
    }
}

void NetGameMode::update(float) {
    if (subsystem<NetSystem>()->isServer() && !server_started_) {
        serverOnStart();
        server_started_ = true;
    }
    if (!subsystem<NetSystem>()->isServer() && server_started_) {
        serverOnEnd();
        server_started_ = false;
    }
}

void NetGameMode::eventOnJoinServer(const JoinServerEvent&) {
    clientOnJoinServer();
}

void NetGameMode::eventOnServerClientConnected(const ServerClientConnectedEvent& e) {
    serverOnClientConnected();
}

void NetGameMode::eventOnServerClientDisconnected(const ServerClientDisconnectedEvent& e) {
    serverOnClientDisconnected();
}

bool NetGameMode::runningAsServer() const {
    return server_started_;
}
}  // namespace dw
