/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

namespace dw {

class DW_API Track
{
public:
    // TODO: replace filename with dw::sound::Source
    Track(const String& filename, irrklang::ISoundEngine* engine);
    ~Track();

    /// Start playing the track
    void play();

    /// Pause the track
    void pause();

    /// Stop the track
    void stop();

    /// Seek to a particular location in the track
    /// @param location Location in the track in the range [0..1]
    void seek(float location);

    /// Set the volume of this track
    /// @param volume Volume percentage in the range [0..1]
    void setVolume(float volume);

private:
    irrklang::ISound* mTrack;
};

}
