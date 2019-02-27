/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#pragma once

using dw::String;
using dw::uint;

enum StateID { S_SANDBOX = dw::S_USER_ID_BEGIN };

// The state where the universe can be explored. Useful for debugging the planetary engine
class SandboxState : public dw::State {
public:
    SandboxState(dw::Engine* engine);
    virtual ~SandboxState();

    void handleEvent(dw::EventDataPtr eventData);

    // Inherited from State
    virtual void enter();
    virtual void exit();
    virtual void update(float dt);
    virtual uint getId() const {
        return S_SANDBOX;
    }
    virtual String getName() const {
        return "SandboxState";
    }

private:
    dw::Engine* mEngine;
    dw::DefaultCamera* mCamera;
    dw::Layout* mData;

    double mTime;
    double mDeltaTime;
    dw::SystemBody* mTrackedObject;

    void UpdateUI(float speed);
};
