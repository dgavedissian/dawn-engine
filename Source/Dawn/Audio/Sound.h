/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

NAMESPACE_BEGIN

class Camera;

// Handles a single sound object - can share sound buffers between instances
class DW_API Sound
{
public:
    Sound(const string& filename, bool looped, irrklang::ISoundEngine* engine);
    Sound(const string& filename, const Position& position, bool looped,
          irrklang::ISoundEngine* engine);
    ~Sound();

    // Playback
    void Play();
    void Pause();
    void Stop();
    bool IsPaused() const;
    bool IsFinished() const;

    /// Set the playback speed
    /// @param speed Speed as a percentage of the default speed
    void SetPlaybackSpeed(float speed);

    /// Set the position of the sound in 3D space
    /// Note that this only works if the sound is 3D
    void SetPosition(const Position& position);
    void SetMinDistance(float minDistance, float attenuation = 0.5f);

    /// Updates the internal sound position
    /// @param dt Frame-time
    void Update(Camera* camera, float dt);

    // Volume
    void SetVolume(float volume);    // must be in the range [0;1]
    float GetVolume() const;

private:
    irrklang::ISound* mSound;

    // 3D stuff
    bool mIs3D;
    Position mPosition;
    Position mOldPosition;
};

NAMESPACE_END
