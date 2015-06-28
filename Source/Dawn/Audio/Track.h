/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

NAMESPACE_BEGIN

class DW_API Track
{
public:
    // TODO: replace filename with dw::sound::Source
    Track(const string& filename, irrklang::ISoundEngine* engine);
    ~Track();

    /// Start playing the track
    void Play();

    /// Pause the track
    void Pause();

    /// Stop the track
    void Stop();

    /// Seek to a particular location in the track
    /// @param location Location in the track in the range [0..1]
    void Seek(float location);

    /// Set the volume of this track
    /// @param volume Volume percentage in the range [0..1]
    void SetVolume(float volume);

private:
    irrklang::ISound* mTrack;
};

NAMESPACE_END
