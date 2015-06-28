/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#include "Common.h"
#include "AsteroidBelt.h"
#include "Barycentre.h"
#include "Planet.h"
#include "Star.h"

NAMESPACE_BEGIN

SystemBody::SystemBody(RenderSystem* rs)
    : mRenderSystem(rs),
      mOrbit(nullptr),
      mParent(nullptr),
      mPosition()
{
}

SystemBody::~SystemBody()
{
    RemoveAllAsteroidBelts();
    RemoveAllSatellites();
}

void SystemBody::SetOrbit(shared_ptr<Orbit> orbit)
{
    mOrbit = orbit;
}

void SystemBody::AddSatellite(shared_ptr<SystemBody> satellite, shared_ptr<Orbit> orbit)
{
    mSatellites.push_back(satellite);
    satellite->SetOrbit(orbit);
    satellite->SetParent(this);
}

void SystemBody::RemoveAllSatellites()
{
    mSatellites.clear();
}

void SystemBody::AddAsteroidBelt(shared_ptr<AsteroidBelt> belt)
{
    mAsteroidBelts.push_back(belt);
    belt->SetParent(this);
}

void SystemBody::RemoveAllAsteroidBelts()
{
    mAsteroidBelts.clear();
}

void SystemBody::Update(float dt, const Position& cameraPosition)
{
    // Update satellites
    for (auto satellite : mSatellites)
        satellite->Update(dt, cameraPosition);

    // Update asteroid belts
    for (auto asteroidBelt : mAsteroidBelts)
        asteroidBelt->Update(dt, cameraPosition);
}

void SystemBody::PreRender(Camera* camera)
{
    // Update satellites
    for (auto satellite : mSatellites)
        satellite->PreRender(camera);

    // Update asteroid belts
    for (auto asteroidBelt : mAsteroidBelts)
        asteroidBelt->PreRender(camera);
}

const Orbit* SystemBody::GetOrbit() const
{
    return mOrbit.get();
}

const SystemBody* SystemBody::GetSatellite(uint index) const
{
    assert(index < mSatellites.size());
    return mSatellites[index].get();
}

const vector<shared_ptr<SystemBody>>& SystemBody::GetAllSatellites() const
{
    return mSatellites;
}

const Position& SystemBody::GetPosition() const
{
    return mPosition;
}

void SystemBody::SetParent(SystemBody* parent)
{
    mParent = parent;
}

void SystemBody::CalculatePosition(double time)
{
    // Get offset
    Position offset;
    if (mParent)
        offset = mParent->GetPosition();

    // Calculate final position
    mPosition = offset;
    if (mOrbit)
        mPosition += mOrbit->CalculatePosition(time);

    // Update satellites
    for (auto i = mSatellites.begin(); i != mSatellites.end(); ++i)
        (*i)->CalculatePosition(time);
}

NAMESPACE_END
