/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "GameMode.h"

namespace dw {
GameMode::GameMode(Context* ctx, GameSession* session) : Object(ctx), session_(session) {
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
