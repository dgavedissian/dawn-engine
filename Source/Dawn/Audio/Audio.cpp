/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#include "Common.h"
#include "Scene/Camera.h"
#include "Audio.h"

NAMESPACE_BEGIN

Audio::Audio() : mSoundEngine(nullptr), mCurrentTrack(nullptr), mLastCameraPosition()
{
    mSoundEngine = irrklang::createIrrKlangDevice(
        irrklang::ESOD_AUTO_DETECT, irrklang::ESEO_MULTI_THREADED | irrklang::ESEO_LOAD_PLUGINS |
                                        irrklang::ESEO_USE_3D_BUFFERS);
}

Audio::~Audio()
{
    mSounds.clear();
    mTracks.clear();
    mSoundEngine->drop();
}

void Audio::PlayTrack(const String& filename)
{
    if (mCurrentTrack)
    {
        DestroyTrack(mCurrentTrack);
    }

    // Attempt to play the next track
    try
    {
        mCurrentTrack = CreateTrack(filename);
        mCurrentTrack->Play();
    }
    catch (std::runtime_error& e)
    {
        LOG << "Runtime Error: " << e.what();
        DestroyTrack(mCurrentTrack);
        mCurrentTrack = nullptr;
    }
}

void Audio::PlaySound(const String& filename)
{
    Sound* sound = CreateSound(filename);
    sound->Play();
    mImmediateSounds.push_back(sound);
}

void Audio::PlaySound(const String& filename, const Position& position, float minDistance,
                             float attenuation /*= 0.5f*/)
{
    Sound* sound = CreateSound(filename, position);
    sound->SetMinDistance(minDistance);
    sound->Play();
    mImmediateSounds.push_back(sound);
}

void Audio::Update(float dt, Camera* Listener)
{
    // Calculate velocity
    Vec3 velocity = dt > math::eps
                        ? Listener->GetPosition().GetRelativeToPoint(mLastCameraPosition) / dt
                        : Vec3(0.0f, 0.0f, 0.0f);
    mLastCameraPosition = Listener->GetPosition();

    // Update Listener orientation
    irrklang::vec3df lookDir = Listener->GetOrientation() * Vec3(0.0f, 0.0f, -1.0f);
    irrklang::vec3df upDir = Listener->GetOrientation() * Vec3(0.0f, 1.0f, 0.0f);
    mSoundEngine->setListenerPosition(irrklang::vec3df(0.0f, 0.0f, 0.0f), lookDir, velocity, upDir);

    // Clear immediate sounds
    auto i = mImmediateSounds.begin();
    while (i != mImmediateSounds.end())
    {
        if ((*i)->IsFinished())
        {
            DestroySound(*i);
            i = mImmediateSounds.erase(i);
        }
        else
        {
            i++;
        }
    }

    // Update sounds
    for (auto snd : mSounds)
        snd->Update(Listener, dt);
}

irrklang::ISoundEngine* Audio::GetSoundEngine()
{
    return mSoundEngine;
}

Track* Audio::CreateTrack(const String& filename)
{
    SharedPtr<Track> track = MakeShared<Track>("Media/sounds/" + filename, mSoundEngine);
    mTracks.push_back(track);
    return track.get();
}

void Audio::DestroyTrack(Track* track)
{
    // Swap and pop
    auto i = mTracks.begin();
    while (i != mTracks.end())
    {
        if ((*i).get() == track)
        {
            *i = std::move(mTracks.back());
            mTracks.pop_back();
            break;
        }

        i++;
    }
}

Sound* Audio::CreateSound(const String& filename, bool looped /*= false*/)
{
    SharedPtr<Sound> sound = MakeShared<Sound>("Media/sounds/" + filename, looped, mSoundEngine);
    mSounds.push_back(sound);
    return sound.get();
}

Sound* Audio::CreateSound(const String& filename, const Position& position,
                                 bool looped /*= false*/)
{
    SharedPtr<Sound> sound =
        MakeShared<Sound>("Media/sounds/" + filename, position, looped, mSoundEngine);
    mSounds.push_back(sound);
    return sound.get();
}

void Audio::DestroySound(Sound* sound)
{
    // Swap and pop
    auto i = mSounds.begin();
    while (i != mSounds.end())
    {
        if ((*i).get() == sound)
        {
            *i = std::move(mSounds.back());
            mSounds.pop_back();
            break;
        }

        i++;
    }
}

NAMESPACE_END
