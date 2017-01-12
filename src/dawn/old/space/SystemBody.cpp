/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "AsteroidBelt.h"
#include "Barycentre.h"
#include "Planet.h"
#include "Star.h"

namespace dw {

SystemBody::SystemBody(Renderer* rs)
    : mRenderSystem(rs), mOrbit(nullptr), mParent(nullptr), mPosition() {
}

SystemBody::~SystemBody() {
    removeAllAsteroidBelts();
    removeAllSatellites();
}

void SystemBody::setOrbit(SharedPtr<Orbit> orbit) {
    mOrbit = orbit;
}

void SystemBody::addSatellite(SharedPtr<SystemBody> satellite, SharedPtr<Orbit> orbit) {
    mSatellites.push_back(satellite);
    satellite->setOrbit(orbit);
    satellite->setParent(this);
}

void SystemBody::removeAllSatellites() {
    mSatellites.clear();
}

void SystemBody::addAsteroidBelt(SharedPtr<AsteroidBelt> belt) {
    mAsteroidBelts.push_back(belt);
    belt->setParent(this);
}

void SystemBody::removeAllAsteroidBelts() {
    mAsteroidBelts.clear();
}

void SystemBody::update(float dt, const Position& cameraPosition) {
    // Update satellites
    for (auto satellite : mSatellites)
        satellite->update(dt, cameraPosition);

    // Update asteroid belts
    for (auto asteroidBelt : mAsteroidBelts)
        asteroidBelt->update(dt, cameraPosition);
}

void SystemBody::preRender(Camera* camera) {
    // Update satellites
    for (auto satellite : mSatellites)
        satellite->preRender(camera);

    // Update asteroid belts
    for (auto asteroidBelt : mAsteroidBelts)
        asteroidBelt->preRender(camera);
}

const Orbit* SystemBody::getOrbit() const {
    return mOrbit.get();
}

const SystemBody* SystemBody::getSatellite(uint index) const {
    assert(index < mSatellites.size());
    return mSatellites[index].get();
}

const Vector<SharedPtr<SystemBody>>& SystemBody::getAllSatellites() const {
    return mSatellites;
}

const Position& SystemBody::getPosition() const {
    return mPosition;
}

void SystemBody::setParent(SystemBody* parent) {
    mParent = parent;
}

void SystemBody::calculatePosition(double time) {
    // Get offset
    Position offset;
    if (mParent)
        offset = mParent->getPosition();

    // Calculate final position
    mPosition = offset;
    if (mOrbit)
        mPosition += mOrbit->calculatePosition(time);

    // Update satellites
    for (auto i = mSatellites.begin(); i != mSatellites.end(); ++i)
        (*i)->calculatePosition(time);
}
}
