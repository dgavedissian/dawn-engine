/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "scene/Camera.h"
#include "Audio.h"

namespace dw {

Audio::Audio() : mSoundEngine(nullptr), mCurrentTrack(nullptr), mLastCameraPosition() {
    mSoundEngine = irrklang::createIrrKlangDevice(
        irrklang::ESOD_AUTO_DETECT, irrklang::ESEO_MULTI_THREADED | irrklang::ESEO_LOAD_PLUGINS |
                                        irrklang::ESEO_USE_3D_BUFFERS);
}

Audio::~Audio() {
    mSounds.clear();
    mTracks.clear();
    mSoundEngine->drop();
}

void Audio::playTrack(const String& filename) {
    if (mCurrentTrack) {
        destroyTrack(mCurrentTrack);
    }

    // Attempt to play the next track
    try {
        mCurrentTrack = createTrack(filename);
        mCurrentTrack->play();
    } catch (std::runtime_error& e) {
        LOG << "Runtime Error: " << e.what();
        destroyTrack(mCurrentTrack);
        mCurrentTrack = nullptr;
    }
}

void Audio::playSound(const String& filename) {
    Sound* sound = createSound(filename);
    sound->play();
    mImmediateSounds.push_back(sound);
}

void Audio::playSound(const String& filename, const Position& position, float minDistance,
                      float attenuation /*= 0.5f*/) {
    Sound* sound = createSound(filename, position);
    sound->setMinDistance(minDistance);
    sound->play();
    mImmediateSounds.push_back(sound);
}

void Audio::update(float dt, Camera* listener) {
    // Calculate velocity
    Vec3 velocity = dt > M_EPSILON ? listener->getPosition().getRelativeTo(mLastCameraPosition) / dt
                                   : Vec3(0.0f, 0.0f, 0.0f);
    mLastCameraPosition = listener->getPosition();

    // Update Listener orientation
    irrklang::vec3df lookDir = listener->getOrientation() * Vec3(0.0f, 0.0f, -1.0f);
    irrklang::vec3df upDir = listener->getOrientation() * Vec3(0.0f, 1.0f, 0.0f);
    mSoundEngine->setListenerPosition(irrklang::vec3df(0.0f, 0.0f, 0.0f), lookDir, velocity, upDir);

    // Clear immediate sounds
    auto i = mImmediateSounds.begin();
    while (i != mImmediateSounds.end()) {
        if ((*i)->isFinished()) {
            destroySound(*i);
            i = mImmediateSounds.erase(i);
        } else {
            i++;
        }
    }

    // Update sounds
    for (auto snd : mSounds)
        snd->update(listener, dt);
}

irrklang::ISoundEngine* Audio::getSoundEngine() {
    return mSoundEngine;
}

Track* Audio::createTrack(const String& filename) {
    SharedPtr<Track> track = makeShared<Track>("Media/Sounds/" + filename, mSoundEngine);
    mTracks.push_back(track);
    return track.get();
}

void Audio::destroyTrack(Track* track) {
    // Swap and pop
    auto i = mTracks.begin();
    while (i != mTracks.end()) {
        if ((*i).get() == track) {
            *i = std::move(mTracks.back());
            mTracks.pop_back();
            break;
        }

        i++;
    }
}

Sound* Audio::createSound(const String& filename, bool looped /*= false*/) {
    SharedPtr<Sound> sound = makeShared<Sound>("media/sounds/" + filename, looped, mSoundEngine);
    mSounds.push_back(sound);
    return sound.get();
}

Sound* Audio::createSound(const String& filename, const Position& position,
                          bool looped /*= false*/) {
    SharedPtr<Sound> sound =
        makeShared<Sound>("media/sounds/" + filename, position, looped, mSoundEngine);
    mSounds.push_back(sound);
    return sound.get();
}

void Audio::destroySound(Sound* sound) {
    // Swap and pop
    auto i = mSounds.begin();
    while (i != mSounds.end()) {
        if ((*i).get() == sound) {
            *i = std::move(mSounds.back());
            mSounds.pop_back();
            break;
        }

        i++;
    }
}
}
