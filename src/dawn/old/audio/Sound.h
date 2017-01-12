/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

namespace dw {

class Camera;

// Handles a single sound object - can share sound buffers between instances
class DW_API Sound {
public:
    Sound(const String& filename, bool looped, irrklang::ISoundEngine* engine);
    Sound(const String& filename, const Position& position, bool looped,
          irrklang::ISoundEngine* engine);
    ~Sound();

    // Playback
    void play();
    void pause();
    void stop();
    bool isPaused() const;
    bool isFinished() const;

    /// Set the playback speed
    /// @param speed Speed as a percentage of the default speed
    void setSpeed(float speed);

    /// Set the position of the sound in 3D space
    /// Note that this only works if the sound is 3D
    void setPosition(const Position& position);
    void setMinDistance(float minDistance, float attenuation = 0.5f);

    /// Updates the internal sound position
    /// @param dt Frame-time
    void update(Camera* camera, float dt);

    // Volume
    void setVolume(float volume);  // must be in the range [0;1]
    float getVolume() const;

private:
    irrklang::ISound* mSound;

    // 3D stuff
    bool mIs3D;
    Position mPosition;
    Position mOldPosition;
};
}
