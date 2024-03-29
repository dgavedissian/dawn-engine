/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "core/GameMode.h"

namespace dw {
class DW_API NetGameMode : public GameMode {
public:
    DW_OBJECT(NetGameMode)

    NetGameMode(Context* ctx, GameSession* session);
    virtual ~NetGameMode();

    /// Called on the client when it joins a server.
    virtual void clientOnJoinServer();

    /// Called on the server when its started.
    virtual void serverOnStart();

    /// Called on the server when its ended.
    virtual void serverOnEnd();

    /// Called on the server when a client connects.
    virtual void serverOnClientConnected();

    /// Called on the server when a client disconnects.
    virtual void serverOnClientDisconnected();

    /// Returns true if this game mode is running on a server, otherwise it's running on a client.
    bool runningAsServer() const;

    // GameMode
    virtual void onStart() override;
    virtual void onEnd() override;
    virtual void update(float dt) override;

private:
    void eventOnJoinServer(const JoinServerEvent&);
    void eventOnServerClientConnected(const ServerClientConnectedEvent& e);
    void eventOnServerClientDisconnected(const ServerClientDisconnectedEvent& e);

    bool server_started_;
};
}  // namespace dw
