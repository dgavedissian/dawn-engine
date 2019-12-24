/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "Layout.h"
#include "Console.h"

namespace dw {

class Renderer;
class Input;
class RocketInterface;
class ImGuiInterface;

// Handles the graphical user interface
class DW_API UI : public Rocket::Core::EventListener, public Ogre::RenderQueueListener {
public:
    UI(Renderer* rs, Input* im, LuaState* ls);
    ~UI();

    /// Called at the beginning of a frame
    void beginFrame();

    // Update
    void update(float dt);
    void preRender();

    // Load a layout
    Layout* loadLayout(const String& filename);
    void unloadLayout(Layout* layout);

    // Event Delegate
    void handleEvent(EventDataPtr eventData);

    // Inherited from Rocket::Core::EventListener
    virtual void ProcessEvent(Rocket::Core::Event& event) override;

    // Inherited from Ogre::RenderQueueListener
    virtual void renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String&, bool&) override;

    RocketInterface* getRocketInterface() {
        return mRocketInterface.get();
    }

private:
    Renderer* mRenderSystem;
    Ogre::MaterialPtr mUIMaterial;
    Ogre::Matrix4 mProjection;

    Rocket::Core::Context* mContext;

    SharedPtr<RocketInterface> mRocketInterface;
    SharedPtr<ImGuiInterface> mImGuiInterface;

    // Console
    SharedPtr<Console> mConsole;

private:
    void buildProjMatrix(Ogre::Matrix4& projectionMatrix);
};

class DW_API EvtData_UIMouseEnter : public EventData {
public:
    typedef Map<String, String> Parameters;
    static const EventType eventType;
    EvtData_UIMouseEnter(const String& _id, const Parameters& p) : id(_id), parameters(p) {
    }
    const EventType& getType() const override {
        return eventType;
    }
    const String getName() const override {
        return "EvtData_UIMouseOver";
    }

    String id;
    Parameters parameters;
};

class DW_API EvtData_UIMouseLeave : public EventData {
public:
    typedef Map<String, String> Parameters;
    static const EventType eventType;
    EvtData_UIMouseLeave(const String& _id, const Parameters& p) : id(_id), parameters(p) {
    }
    const EventType& getType() const override {
        return eventType;
    }
    const String getName() const override {
        return "EvtData_UIMouseLeave";
    }

    String id;
    Parameters parameters;
};

class DW_API EvtData_UIClick : public EventData {
public:
    typedef Map<String, String> Parameters;
    static const EventType eventType;
    EvtData_UIClick(const String& _id, const Parameters& p) : id(_id), parameters(p) {
    }
    const EventType& getType() const override {
        return eventType;
    }
    const String getName() const override {
        return "EvtData_UIClick";
    }

    String id;
    Parameters parameters;
};

class DW_API EvtData_UISubmit : public EventData {
public:
    typedef Map<String, String> Parameters;
    static const EventType eventType;
    EvtData_UISubmit(const String& _id, const Parameters& p) : id(_id), parameters(p) {
    }
    const EventType& getType() const override {
        return eventType;
    }
    const String getName() const override {
        return "EvtData_UISubmit";
    }

    String id;
    Parameters parameters;
};
}  // namespace dw
