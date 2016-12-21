/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "Audio.h"
#include "Sound.h"

namespace dw {

Sound::Sound(const String& filename, bool looped, irrklang::ISoundEngine* engine)
    : mIs3D(false), mPosition(Position::origin), mOldPosition(Position::origin) {
    mSound = engine->play2D(filename.c_str(), looped, true, true);
}

Sound::Sound(const String& filename, const Position& position, bool looped,
             irrklang::ISoundEngine* engine)
    : mIs3D(true), mPosition(position), mOldPosition(Position::origin) {
    mSound = engine->play3D(filename.c_str(), Vec3::zero, looped, true, true);
}

Sound::~Sound() {
    mSound->stop();
    mSound->drop();
}

void Sound::play() {
    mSound->setIsPaused(false);
}

void Sound::stop() {
    pause();
    mSound->setPlayPosition(0);
}

void Sound::pause() {
    mSound->setIsPaused(true);
}

bool Sound::isPaused() const {
    return mSound->getIsPaused();
}

bool Sound::isFinished() const {
    return mSound->isFinished();
}

void Sound::setSpeed(float speed) {
    mSound->setPlaybackSpeed(speed);
}

void Sound::setPosition(const Position& position) {
    mPosition = position;
}

void Sound::setMinDistance(float minDistance, float attenuation /*= 0.5f*/) {
    mSound->setMinDistance(minDistance);
}

void Sound::update(Camera* camera, float dt) {
    if (mIs3D) {
        Vec3 velocity = dt > M_EPSILON ? mPosition.getRelativeTo(mOldPosition) / dt : Vec3(0.0f);
        mOldPosition = mPosition;
        mSound->setVelocity(velocity);
        mSound->setPosition(mPosition.toCameraSpace(camera));
    }
}

void Sound::setVolume(float volume) {
    mSound->setVolume(volume);
}

float Sound::getVolume() const {
    return mSound->getVolume();
}
}
