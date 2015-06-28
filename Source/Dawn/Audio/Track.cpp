/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#include "Common.h"
#include "Track.h"

NAMESPACE_BEGIN

Track::Track(const string& filename, irrklang::ISoundEngine* engine)
{
    // Load the track
    mTrack = engine->play2D(filename.c_str(), true, true, true);
    if (!mTrack)
        throw std::runtime_error(("ERROR: File " + filename + " not found!").c_str());

    // Set it to loop
    mTrack->setIsLooped(true);
}

Track::~Track()
{
    mTrack->stop();
    mTrack->drop();
}

void Track::Play()
{
    mTrack->setIsPaused(false);
}

void Track::Pause()
{
    mTrack->setIsPaused(true);
}

void Track::Stop()
{
    Pause();
    mTrack->setPlayPosition(0);
}

void Track::Seek(float location)
{
    irrklang::ik_u32 length = mTrack->getSoundSource()->getPlayLength();
    if (length > 0.0f)
        mTrack->setPlayPosition((irrklang::ik_u32)(location * length));
}

void Track::SetVolume(float volume)
{
    mTrack->setVolume(volume);
}

NAMESPACE_END
