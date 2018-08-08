/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

namespace dw {
class SceneManager;

class DW_API GameMode : public Object {
public:
    DW_OBJECT(GameMode)

    GameMode(Context* ctx, SceneManager* scene_manager);
    virtual ~GameMode();

    /// Called when the game mode is started.
    virtual void onStart();

    /// Called when the game mode ends.
    virtual void onEnd();

    /// Updates this game mode.
    /// @param dt Delta time.
    virtual void update(float dt);

protected:
    SceneManager* scene_manager_;
};
}  // namespace dw
