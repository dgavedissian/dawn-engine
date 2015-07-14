/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

#include "Sound.h"
#include "Track.h"

NAMESPACE_BEGIN

class Camera;

// Manages IrrKlang and provides some helper functions
class DW_API Audio
{
public:
    Audio();
    ~Audio();

    // ---------------------------------------------------
    // Immediate mode interface

    // TODO: replace with Playlist
    /// Plays a new track immediately
    /// @param filename The filename of the track to be played
    DEPRECATED void PlayTrack(const string& filename);

    /// Shortcut function to play a 2D sound immediately
    /// @param filename The filename of the sound to be played
    DEPRECATED void PlaySound(const string& filename);

    /// Shortcut function to play a 3D sound immediately
    /// @param filename The filename of the sound to be played
    /// @param position The position for the sound to appear from
    /// @param filename The filename of the sound to be played
    /// @param filename The filename of the sound to be played
    DEPRECATED void PlaySound(const string& filename, const Position& position, float minDistance,
                   float attenuation = 0.5f);

    // ---------------------------------------------------

    /// Creates a track
    /// @param filename The filename of the track to be played
    Track* CreateTrack(const string& filename);

    /// Destroys a track
    /// @param track Track to destroy
    void DestroyTrack(Track* track);

    /// Creates a new 2D sound
    /// @param filename The filename of the sound
    /// @param loop     True if the sound should be looped
    Sound* CreateSound(const string& filename, bool looped = false);

    /// Creates a new 3D sound
    /// @param filename The filename of the sound
    /// @param position Position of the sound
    /// @param loop     True if the sound should be looped
    Sound* CreateSound(const string& filename, const Position& position, bool looped = false);

    /// Destroys a sound
    /// @param sound The sound to destroy
    void DestroySound(Sound* sound);

    /// Updates the listeners position
    /// @param dt          Delta-time
    /// @param listener    Listener
    void Update(float dt, Camera* listener);

    /// Returns the current irrKlang sound engine
    /// @return The pointer to the irrKlang sound engine
private:
    irrklang::ISoundEngine* GetSoundEngine();

private:
    irrklang::ISoundEngine* mSoundEngine;

    std::vector<shared_ptr<Sound>> mSounds;
    std::vector<shared_ptr<Track>> mTracks;

    // Immediate mode stuff
    Track* mCurrentTrack;
    std::vector<Sound*> mImmediateSounds;

    // Camera velocity
    Position mLastCameraPosition;

};

NAMESPACE_END
