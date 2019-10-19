/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "Track.h"

namespace dw {

Track::Track(const String& filename, irrklang::ISoundEngine* engine) {
    // Load the track
    mTrack = engine->play2D(filename.c_str(), true, true, true);
    if (!mTrack)
        throw std::runtime_error(("ERROR: File " + filename + " not found!").c_str());

    // Set it to loop
    mTrack->setIsLooped(true);
}

Track::~Track() {
    mTrack->stop();
    mTrack->drop();
}

void Track::play() {
    mTrack->setIsPaused(false);
}

void Track::pause() {
    mTrack->setIsPaused(true);
}

void Track::stop() {
    pause();
    mTrack->setPlayPosition(0);
}

void Track::seek(float location) {
    irrklang::ik_u32 length = mTrack->getSoundSource()->getPlayLength();
    if (length > 0.0f)
        mTrack->setPlayPosition((irrklang::ik_u32)(location * length));
}

void Track::setVolume(float volume) {
    mTrack->setVolume(volume);
}
}  // namespace dw
