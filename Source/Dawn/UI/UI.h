/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

#include "Layout.h"
#include "Console.h"

NAMESPACE_BEGIN

class Renderer;
class Input;
class RocketInterface;
class ImGuiInterface;

// Handles the graphical user interface
class DW_API UI : public Rocket::Core::EventListener, public Ogre::RenderQueueListener
{
public:
    UI(Renderer* rs, Input* im, LuaState* ls);
    ~UI();

    /// Called at the beginning of a frame
    void BeginFrame();

    // Update
    void Update(float dt);
    void PreRender();

    // Load a layout
    Layout* LoadLayout(const string& filename);
    void UnloadLayout(Layout* layout);

    // Event Delegate
    void HandleEvent(EventDataPtr eventData);

    // Inherited from Rocket::Core::EventListener
	virtual void ProcessEvent(Rocket::Core::Event& event) override;

    // Inherited from Ogre::RenderQueueListener
    virtual void renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String&, bool&) override;

    RocketInterface* getRocketInterface() { return mRocketInterface.get(); }

private:
    Renderer* mRenderSystem;

    Rocket::Core::Context* mContext;

    shared_ptr<RocketInterface> mRocketInterface;
    shared_ptr<ImGuiInterface> mImGuiInterface;

    // Console
    shared_ptr<Console> mConsole;

    // Internal: Configure the render system to draw libRocket controls
    void ConfigureRenderSystem();
    void BuildProjectionMatrix(Ogre::Matrix4& projectionMatrix);
};

class EvtData_UIClick : public EventData
{
public:
    typedef std::map<string, string> Parameters;
    static const EventType eventType;
    EvtData_UIClick(const string& _id, const Parameters& p) : id(_id), parameters(p) {}
    const EventType& GetEventType() const override { return eventType; }
    const string GetName() const override { return "EvtData_UIClick"; }

    string id;
    Parameters parameters;
};

class EvtData_UISubmit : public EventData
{
public:
    typedef std::map<string, string> Parameters;
    static const EventType eventType;
    EvtData_UISubmit(const string& _id, const Parameters& p) : id(_id), parameters(p) {}
    const EventType& GetEventType() const override { return eventType; }
    const string GetName() const override { return "EvtData_UISubmit"; }

    string id;
    Parameters parameters;
};

NAMESPACE_END
