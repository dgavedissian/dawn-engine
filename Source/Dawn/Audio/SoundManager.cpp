/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#include "Common.h"
#include "Scene/Camera.h"
#include "SoundManager.h"

NAMESPACE_BEGIN

SoundManager::SoundManager() : mSoundEngine(nullptr), mCurrentTrack(nullptr), mLastCameraPosition()
{
    mSoundEngine = irrklang::createIrrKlangDevice(
        irrklang::ESOD_AUTO_DETECT, irrklang::ESEO_MULTI_THREADED | irrklang::ESEO_LOAD_PLUGINS |
                                        irrklang::ESEO_USE_3D_BUFFERS);
}

SoundManager::~SoundManager()
{
    mSounds.clear();
    mTracks.clear();
    mSoundEngine->drop();
}

void SoundManager::PlayTrack(const string& filename)
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

void SoundManager::PlaySound(const string& filename)
{
    Sound* sound = CreateSound(filename);
    sound->Play();
    mImmediateSounds.push_back(sound);
}

void SoundManager::PlaySound(const string& filename, const Position& position, float minDistance,
                             float attenuation /*= 0.5f*/)
{
    Sound* sound = CreateSound(filename, position);
    sound->SetMinDistance(minDistance);
    sound->Play();
    mImmediateSounds.push_back(sound);
}

void SoundManager::Update(float dt, Camera* listener)
{
    // Calculate velocity
    Vec3 velocity = dt > math::eps
                        ? listener->GetPosition().GetRelativeToPoint(mLastCameraPosition) / dt
                        : Vec3(0.0f, 0.0f, 0.0f);
    mLastCameraPosition = listener->GetPosition();

    // Update listener orientation
    irrklang::vec3df lookDir = listener->GetOrientation() * Vec3(0.0f, 0.0f, -1.0f);
    irrklang::vec3df upDir = listener->GetOrientation() * Vec3(0.0f, 1.0f, 0.0f);
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
        snd->Update(listener, dt);
}

irrklang::ISoundEngine* SoundManager::GetSoundEngine()
{
    return mSoundEngine;
}

Track* SoundManager::CreateTrack(const string& filename)
{
    shared_ptr<Track> track = make_shared<Track>("Media/sounds/" + filename, mSoundEngine);
    mTracks.push_back(track);
    return track.get();
}

void SoundManager::DestroyTrack(Track* track)
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

Sound* SoundManager::CreateSound(const string& filename, bool looped /*= false*/)
{
    shared_ptr<Sound> sound = make_shared<Sound>("Media/sounds/" + filename, looped, mSoundEngine);
    mSounds.push_back(sound);
    return sound.get();
}

Sound* SoundManager::CreateSound(const string& filename, const Position& position,
                                 bool looped /*= false*/)
{
    shared_ptr<Sound> sound =
        make_shared<Sound>("Media/sounds/" + filename, position, looped, mSoundEngine);
    mSounds.push_back(sound);
    return sound.get();
}

void SoundManager::DestroySound(Sound* sound)
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
