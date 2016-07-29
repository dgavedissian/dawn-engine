/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "Orbit.h"
#include "AsteroidBelt.h"

namespace dw {
class Renderer;


// Base class for any bodies inside a planetary system
class DW_API SystemBody : public EnableSharedFromThis<SystemBody>
{
public:
    SystemBody(Renderer* rs);
    virtual ~SystemBody();

    // Set the orbit of this body
    void setOrbit(SharedPtr<Orbit> orbit);

    // Add a satellite
    void addSatellite(SharedPtr<SystemBody> satellite, SharedPtr<Orbit> orbit);

    // Remove all satellites
    void removeAllSatellites();

    // Add an asteroid belt
    void addAsteroidBelt(SharedPtr<AsteroidBelt> belt);

    // Remove all asteroid belts
    void removeAllAsteroidBelts();

    // Update this body
    virtual void update(float dt, const Position& cameraPosition);

    // Pre-render this body
    virtual void preRender(Camera* camera);

    // Accessors
    const Orbit* getOrbit() const;
    const SystemBody* getSatellite(uint index) const;
    const Vector<SharedPtr<SystemBody>>& getAllSatellites() const;
    const Position& getPosition() const;

    // Internal: Set the parent of this SystemBody
    DEPRECATED void setParent(SystemBody* parent);

    // Internal: Calculate the position of this object and all satellites
    // at a specific time
    virtual void calculatePosition(double time);

protected:
    Renderer* mRenderSystem;

    SharedPtr<Orbit> mOrbit;

    Vector<SharedPtr<SystemBody>> mSatellites;
    Vector<SharedPtr<AsteroidBelt>> mAsteroidBelts;
    SystemBody* mParent;

    Position mPosition;
};

}
