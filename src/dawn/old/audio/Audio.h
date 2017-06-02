/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "Sound.h"
#include "Track.h"

namespace dw {

class Camera;

// Manages IrrKlang and provides some helper functions
class DW_API Audio {
public:
    Audio();
    ~Audio();

    // ---------------------------------------------------
    // Immediate mode interface

    // TODO: replace with PlayList
    /// Plays a new track immediately
    /// @param filename The filename of the track to be played
    DEPRECATED void playTrack(const String& filename);

    /// Shortcut function to play a 2D sound immediately
    /// @param filename The filename of the sound to be played
    DEPRECATED void playSound(const String& filename);

    /// Shortcut function to play a 3D sound immediately
    /// @param filename The filename of the sound to be played
    /// @param position The position for the sound to appear from
    /// @param filename The filename of the sound to be played
    /// @param filename The filename of the sound to be played
    DEPRECATED void playSound(const String& filename, const Position& position, float minDistance,
                              float attenuation = 0.5f);

    // ---------------------------------------------------

    /// Creates a track
    /// @param filename The filename of the track to be played
    Track* createTrack(const String& filename);

    /// Destroys a track
    /// @param track Track to destroy
    void destroyTrack(Track* track);

    /// Creates a new 2D sound
    /// @param filename The filename of the sound
    /// @param loop     True if the sound should be looped
    Sound* createSound(const String& filename, bool looped = false);

    /// Creates a new 3D sound
    /// @param filename The filename of the sound
    /// @param position Position of the sound
    /// @param loop     True if the sound should be looped
    Sound* createSound(const String& filename, const Position& position, bool looped = false);

    /// Destroys a sound
    /// @param sound The sound to destroy
    void destroySound(Sound* sound);

    /// Updates the Listeners position
    /// @param dt          Delta-time
    /// @param Listener    Listener
    void update(float dt, Camera* Listener);

    /// Returns the current irrKlang sound engine
    /// @return The pointer to the irrKlang sound engine
private:
    irrklang::ISoundEngine* getSoundEngine();

private:
    irrklang::ISoundEngine* mSoundEngine;

    Vector<SharedPtr<Sound>> mSounds;
    Vector<SharedPtr<Track>> mTracks;

    // Immediate mode stuff
    Track* mCurrentTrack;
    Vector<Sound*> mImmediateSounds;

    // Camera velocity
    Position mLastCameraPosition;
};
}  // namespace dw
