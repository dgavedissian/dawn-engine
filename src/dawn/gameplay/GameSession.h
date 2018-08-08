/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "GameMode.h"
#include "net/NetInstance.h"
#include "scene/SceneManager.h"

namespace dw {
struct DW_API GameSessionInfo {
    struct CreateLocalGame {
        String scene_name;
    };

    struct CreateGame {
        String host;  // host to bind to. usually 127.0.0.1.
        u16 port;
        u16 max_clients;
        String scene_name;
        // TODO: Transport.
    };

    struct JoinGame {
        String host;
        u16 port;
        // TODO: Transport.
    };

    Variant<CreateLocalGame, CreateGame, JoinGame> start_info;

    // Other parameters.
    HashMap<String, String> params;
};

class DW_API GameSession : public Object {
public:
    DW_OBJECT(GameSession);

    GameSession(Context* ctx, const GameSessionInfo& gsi);
    virtual ~GameSession();

    virtual void update(float dt);

    // TODO: Deprecate below.

    /// Sets a new game mode.
    void setGameMode(SharedPtr<GameMode> game_mode);

    /// Gets the current game mode.
    GameMode* gameMode() const;

protected:
    UniquePtr<SceneManager> scene_manager_;
    UniquePtr<NetInstance> net_instance_;

private:
    SharedPtr<GameMode> new_game_mode_;
    SharedPtr<GameMode> game_mode_;
};
}  // namespace dw
