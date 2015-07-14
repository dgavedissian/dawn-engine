/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#include "Common.h"
#include "Audio.h"
#include "Sound.h"

NAMESPACE_BEGIN

Sound::Sound(const string& filename, bool looped, irrklang::ISoundEngine* engine)
    : mIs3D(false), mPosition(Position::origin), mOldPosition(Position::origin)
{
    mSound = engine->play2D(filename.c_str(), looped, true, true);
}

Sound::Sound(const string& filename, const Position& position, bool looped,
             irrklang::ISoundEngine* engine)
    : mIs3D(true), mPosition(position), mOldPosition(Position::origin)
{
    mSound = engine->play3D(filename.c_str(), Vec3::zero, looped, true, true);
}

Sound::~Sound()
{
    mSound->stop();
    mSound->drop();
}

void Sound::Play()
{
    mSound->setIsPaused(false);
}

void Sound::Stop()
{
    Pause();
    mSound->setPlayPosition(0);
}

void Sound::Pause()
{
    mSound->setIsPaused(true);
}

bool Sound::IsPaused() const
{
    return mSound->getIsPaused();
}

bool Sound::IsFinished() const
{
    return mSound->isFinished();
}

void Sound::SetPlaybackSpeed(float speed)
{
    mSound->setPlaybackSpeed(speed);
}

void Sound::SetPosition(const Position& position)
{
    mPosition = position;
}

void Sound::SetMinDistance(float minDistance, float attenuation /*= 0.5f*/)
{
    mSound->setMinDistance(minDistance);
}

void Sound::Update(Camera* camera, float dt)
{
    if (mIs3D)
    {
        Vec3 velocity =
            dt > math::eps ? mPosition.GetRelativeToPoint(mOldPosition) / dt : Vec3(0.0f);
        mOldPosition = mPosition;
        mSound->setVelocity(velocity);
        mSound->setPosition(mPosition.ToCameraSpace(camera));
    }
}

void Sound::SetVolume(float volume)
{
    mSound->setVolume(volume);
}

float Sound::GetVolume() const
{
    return mSound->getVolume();
}

NAMESPACE_END
