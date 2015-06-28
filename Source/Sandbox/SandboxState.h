/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

#include "Engine.h"
#include "UI/Layout.h"
#include "Scene/space/SystemBody.h"

NAMESPACE_BEGIN

// The state where the universe can be explored. Useful for debugging the planetary engine
class SandboxState : public State
{
public:
    SandboxState(Engine* engine);
    virtual ~SandboxState();

    void HandleEvent(EventDataPtr eventData);

    // Inherited from State
    virtual void Enter();
    virtual void Exit();
    virtual void Update(float dt);
    virtual uint GetID() const { return S_SANDBOX; }
    virtual string GetName() const { return "SandboxState"; }

private:
    Engine* mEngine;
    Layout* mData;

    double mTime;
    double mDeltaTime;
    SystemBody* mTrackedObject;

    void UpdateUI(float speed);
};

NAMESPACE_END
