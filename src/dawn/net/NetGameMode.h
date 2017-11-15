/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "gameplay/GameMode.h"

namespace dw {
class DW_API NetGameMode : public GameMode {
public:
    DW_OBJECT(NetGameMode)

    NetGameMode(Context* ctx);
    virtual ~NetGameMode();

    /// Called on the client when it joins a server.
    virtual void onJoinServer();

    /// Called on the server when a client connects.
    virtual void onClientConnected();

    /// Called on the server when a client disconnects.
    virtual void onClientDisconnected();

    // GameMode
    virtual void onStart() override;
    virtual void onEnd() override;
    virtual void update(float dt) override;

private:
    void eventOnJoinServer(const JoinServerEvent&);
    void eventOnServerClientConnected(const ServerClientConnectedEvent& e);
    void eventOnServerClientDisconnected(const ServerClientDisconnectedEvent& e);
};
}  // namespace dw
