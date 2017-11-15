/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "input/Input.h"
#include "gameplay/GameMode.h"

namespace dw {
class DW_API GameFramework : public Subsystem {
public:
    DW_OBJECT(GameFramework)

    GameFramework(Context* context);
    ~GameFramework();

    /// Sets a new game mode.
    void setGameMode(SharedPtr<GameMode> game_mode);

    /// Updates the game state.
    void update(float dt);

    /// Gets the current game mode.
    GameMode* getGameMode();

private:
    SharedPtr<GameMode> new_game_mode_;
    SharedPtr<GameMode> game_mode_;

    void onKey(const KeyEvent& data);
};
}  // namespace dw
