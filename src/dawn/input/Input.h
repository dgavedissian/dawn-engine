/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "math/Vec2i.h"
#include "math/Vec3i.h"

namespace dw {

class DW_API Input {
public:
    Input();
    ~Input();

    /// Set the viewport size
    /// @param viewportSize Size of the viewport in pixels
    void setViewportSize(const Vec2i& viewportSize);

    /// Toggles the relative mouse mode, which fixes the position and hides
    /// the cursor
    /// @param visible True if relative, false otherwise
    void lockCursor(bool relative);

    // Input blocking
    DEPRECATED void pushInputBlock();
    DEPRECATED void popInputBlock();
    DEPRECATED bool isInputBlocked() const;

    /// Process an SDL event. This should be called only by RenderSystem
    /// @param e SDL event structure
    void handleSDLEvent(SDL_Event& e);

    // Check the current state of the input devices
    bool isKeyDown(SDL_Keycode key) const;
    bool isMouseButtonDown(uint button) const;
    Vec2i getMousePosition() const;
    Vec2 getMousePositionRel() const;
    Vec3i getMouseMove() const;

private:
    uint mInputBlock;
    Vec2i mViewportSize;
};

class DW_API EvtData_KeyDown : public EventData {
public:
    static const EventType eventType;

    EvtData_KeyDown(SDL_Keycode kc, SDL_Scancode sc, u16 m) : keycode(kc), scancode(sc), mod(m) {
    }
    const EventType& getType() const override {
        return eventType;
    }
    const String getName() const override {
        return "EvtData_KeyDown";
    }

    SDL_Keycode keycode;
    SDL_Scancode scancode;
    u16 mod;
};

class DW_API EvtData_KeyUp : public EventData {
public:
    static const EventType eventType;

    EvtData_KeyUp(SDL_Keycode kc, SDL_Scancode sc, u16 m) : keycode(kc), scancode(sc), mod(m) {
    }
    const EventType& getType() const override {
        return eventType;
    }
    const String getName() const override {
        return "EvtData_KeyUp";
    }

    SDL_Keycode keycode;
    SDL_Scancode scancode;
    u16 mod;
};

class DW_API EvtData_TextInput : public EventData {
public:
    static const EventType eventType;

    EvtData_TextInput(String t) : text(t) {
    }
    const EventType& getType() const override {
        return eventType;
    }
    const String getName() const override {
        return "EvtData_TextInput";
    }

    String text;
};

class DW_API EvtData_MouseDown : public EventData {
public:
    static const EventType eventType;

    EvtData_MouseDown(uint b) : button(b) {
    }
    const EventType& getType() const override {
        return eventType;
    }
    const String getName() const override {
        return "EvtData_MouseDown";
    }

    uint button;
};

class DW_API EvtData_MouseUp : public EventData {
public:
    static const EventType eventType;

    EvtData_MouseUp(uint b) : button(b) {
    }
    const EventType& getType() const override {
        return eventType;
    }
    const String getName() const override {
        return "EvtData_MouseUp";
    }

    uint button;
};

class DW_API EvtData_MouseMove : public EventData {
public:
    static const EventType eventType;

    EvtData_MouseMove(const Vec2i& p, const Vec2& pr, const Vec2i& m)
        : pos(p), posRel(pr), motion(m) {
    }
    const EventType& getType() const override {
        return eventType;
    }
    const String getName() const override {
        return "EvtData_MouseMove";
    }

    Vec2i pos;
    Vec2 posRel;
    Vec2i motion;
};

class DW_API EvtData_MouseWheel : public EventData {
public:
    static const EventType eventType;

    EvtData_MouseWheel(const Vec2i& m) : motion(m) {
    }
    const EventType& getType() const override {
        return eventType;
    }
    const String getName() const override {
        return "EvtData_MouseWheel";
    }

    Vec2i motion;
};
}
