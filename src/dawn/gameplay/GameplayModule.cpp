/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "input/Input.h"
#include "GameplayModule.h"

namespace dw {
GameplayModule::GameplayModule(Context* context)
    : Module(context), new_game_mode_(nullptr), game_mode_(nullptr) {
    addEventListener<KeyEvent>(makeEventDelegate(this, &GameplayModule::onKey));
}

GameplayModule::~GameplayModule() {
    removeEventListener<KeyEvent>(makeEventDelegate(this, &GameplayModule::onKey));
}

void GameplayModule::setGameMode(SharedPtr<GameMode> game_mode) {
    new_game_mode_ = game_mode;
}

void GameplayModule::update(float dt) {
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

GameMode* GameplayModule::gameMode() {
    return game_mode_.get();
}

void GameplayModule::onKey(const KeyEvent& data) {
    if (data.key == Key::F8 && data.down) {
        // reload();
    }
}
}  // namespace dw
