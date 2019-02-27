/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
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
