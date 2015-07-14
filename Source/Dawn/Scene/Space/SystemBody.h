/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

#include "Orbit.h"
#include "AsteroidBelt.h"

NAMESPACE_BEGIN
class Renderer;


// Base class for any bodies inside a planetary system
class DW_API SystemBody : public enable_shared_from_this<SystemBody>
{
public:
    SystemBody(Renderer* rs);
    virtual ~SystemBody();

    // Set the orbit of this body
    void SetOrbit(shared_ptr<Orbit> orbit);

    // Add a satellite
    void AddSatellite(shared_ptr<SystemBody> satellite, shared_ptr<Orbit> orbit);

    // Remove all satellites
    void RemoveAllSatellites();

    // Add an asteroid belt
    void AddAsteroidBelt(shared_ptr<AsteroidBelt> belt);

    // Remove all asteroid belts
    void RemoveAllAsteroidBelts();

    // Update this body
    virtual void Update(float dt, const Position& cameraPosition);

    // Pre-render this body
    virtual void PreRender(Camera* camera);

    // Accessors
    const Orbit* GetOrbit() const;
    const SystemBody* GetSatellite(uint index) const;
    const vector<shared_ptr<SystemBody>>& GetAllSatellites() const;
    const Position& GetPosition() const;

    // Internal: Set the parent of this SystemBody
    DEPRECATED void SetParent(SystemBody* parent);

    // Internal: Calculate the position of this object and all satellites
    // at a specific time
    virtual void CalculatePosition(double time);

protected:
    Renderer* mRenderSystem;

    shared_ptr<Orbit> mOrbit;

    vector<shared_ptr<SystemBody>> mSatellites;
    vector<shared_ptr<AsteroidBelt>> mAsteroidBelts;
    SystemBody* mParent;

    Position mPosition;
};

NAMESPACE_END
