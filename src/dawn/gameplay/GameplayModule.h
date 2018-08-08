/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "GameSession.h"

namespace dw {
struct DW_API SessionId {
public:
    SessionId(int session_index);

    u32 index() const;

private:
    u32 session_index_;

    friend class GameplayModule;
};

class DW_API GameplayModule : public Module {
public:
    DW_OBJECT(GameplayModule)

    GameplayModule(Context* context);
    ~GameplayModule();

    SessionId addSession(UniquePtr<GameSession> session);
    void replaceSession(SessionId session_id, UniquePtr<GameSession> session);
    void removeSession(SessionId session_id);

    /// Updates the game state.
    void update(float dt);

private:
    Vector<UniquePtr<GameSession>> game_sessions_;
};
}  // namespace dw
