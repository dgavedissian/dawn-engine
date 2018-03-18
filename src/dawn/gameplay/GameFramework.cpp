/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "input/Input.h"
#include "gameplay/GameFramework.h"

namespace dw {
GameFramework::GameFramework(Context* context)
    : Subsystem(context), new_game_mode_(nullptr), game_mode_(nullptr) {
    addEventListener<KeyEvent>(makeEventDelegate(this, &GameFramework::onKey));
}

GameFramework::~GameFramework() {
    removeEventListener<KeyEvent>(makeEventDelegate(this, &GameFramework::onKey));
}

void GameFramework::setGameMode(SharedPtr<GameMode> game_mode) {
    new_game_mode_ = game_mode;
}

void GameFramework::update(float dt) {
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

GameMode* GameFramework::getGameMode() {
    return game_mode_.get();
}

void GameFramework::onKey(const KeyEvent& data) {
    if (data.key == Key::F8 && data.down) {
        // reload();
    }
}
}  // namespace dw
