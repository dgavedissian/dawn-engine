/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "input/Input.h"
#include "GameplayModule.h"

namespace dw {
SessionId::SessionId(int session_index) : session_index_(session_index) {
}

u32 SessionId::index() const {
    return session_index_;
}

GameplayModule::GameplayModule(Context* context) : Module(context) {
}

GameplayModule::~GameplayModule() {
}

SessionId GameplayModule::addSession(UniquePtr<GameSession> session) {
    // TODO: Initialise session.
    game_sessions_.emplace_back(std::move(session));
    return SessionId(static_cast<u32>(game_sessions_.size() - 1));
}

void GameplayModule::replaceSession(SessionId session_id, UniquePtr<GameSession> session) {
    assert(game_sessions_.size() < session_id.index());
    // TODO: Initialise session.
    game_sessions_[session_id.index()] = std::move(session);
}

void GameplayModule::removeSession(SessionId session_id) {
    game_sessions_[session_id.index()].reset();
}

void GameplayModule::update(float dt) {
    for (auto it = game_sessions_.begin(); it != game_sessions_.end(); ++it) {
        (*it)->update(dt);
    }
}
}  // namespace dw
