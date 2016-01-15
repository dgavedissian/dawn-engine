/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "Orbit.h"
#include "AsteroidBelt.h"

NAMESPACE_BEGIN
class Renderer;


// Base class for any bodies inside a planetary system
class DW_API SystemBody : public EnableSharedFromThis<SystemBody>
{
public:
    SystemBody(Renderer* rs);
    virtual ~SystemBody();

    // Set the orbit of this body
    void SetOrbit(SharedPtr<Orbit> orbit);

    // Add a satellite
    void AddSatellite(SharedPtr<SystemBody> satellite, SharedPtr<Orbit> orbit);

    // Remove all satellites
    void RemoveAllSatellites();

    // Add an asteroid belt
    void AddAsteroidBelt(SharedPtr<AsteroidBelt> belt);

    // Remove all asteroid belts
    void RemoveAllAsteroidBelts();

    // Update this body
    virtual void Update(float dt, const Position& cameraPosition);

    // Pre-render this body
    virtual void PreRender(Camera* camera);

    // Accessors
    const Orbit* GetOrbit() const;
    const SystemBody* GetSatellite(uint index) const;
    const Vector<SharedPtr<SystemBody>>& GetAllSatellites() const;
    const Position& GetPosition() const;

    // Internal: Set the parent of this SystemBody
    DEPRECATED void SetParent(SystemBody* parent);

    // Internal: Calculate the position of this object and all satellites
    // at a specific time
    virtual void CalculatePosition(double time);

protected:
    Renderer* mRenderSystem;

    SharedPtr<Orbit> mOrbit;

    Vector<SharedPtr<SystemBody>> mSatellites;
    Vector<SharedPtr<AsteroidBelt>> mAsteroidBelts;
    SystemBody* mParent;

    Position mPosition;
};

NAMESPACE_END
