/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "GameMode.h"

namespace dw {
GameMode::GameMode(Context* ctx, SceneManager* scene_manager)
    : Object(ctx), scene_manager_(scene_manager) {
}

GameMode::~GameMode() {
}

void GameMode::onStart() {
}

void GameMode::onEnd() {
}

void GameMode::update(float) {
}
}  // namespace dw
