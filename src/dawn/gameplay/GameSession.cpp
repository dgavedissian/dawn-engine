/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "GameSession.h"
#include "net/NetInstance.h"

namespace dw {
GameSession::GameSession(Context* ctx, const GameSessionInfo& gsi)
    : Object(ctx), new_game_mode_(nullptr), game_mode_(nullptr) {
    // Initialise the scene manager.
    scene_manager_ = makeUnique<SceneManager>(ctx);

    // Initialise networking.
    if (gsi.start_info.is<GameSessionInfo::CreateGame>()) {
        auto& info = gsi.start_info.get<GameSessionInfo::CreateGame>();
        net_instance_ =
            NetInstance::listen(ctx, scene_manager_.get(), info.host, info.port, info.max_clients);
    } else if (gsi.start_info.is<GameSessionInfo::JoinGame>()) {
        auto& info = gsi.start_info.get<GameSessionInfo::JoinGame>();
        net_instance_ = NetInstance::connect(ctx, scene_manager_.get(), info.host, info.port);
    }
}

GameSession::~GameSession() {
}

void GameSession::update(float dt) {
    net_instance_->update(dt);
    scene_manager_->update(dt);

    // Update the game mode.
    if (game_mode_) {
        game_mode_->update(dt);
    }
    if (new_game_mode_) {
        if (game_mode_) {
            game_mode_->onEnd();
        }
        game_mode_ = new_game_mode_;
        game_mode_->onStart();
        new_game_mode_.reset();
    }
}

void GameSession::setGameMode(SharedPtr<GameMode> game_mode) {
    new_game_mode_ = game_mode;
}

GameMode* GameSession::gameMode() const {
    return game_mode_.get();
}
}  // namespace dw
