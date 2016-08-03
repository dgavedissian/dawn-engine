/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "math/Vec2i.h"
#include "math/Vec3i.h"

namespace dw {

enum MouseButton
{
    None,
    Left,
    Middle,
    Right,

    Count
};

enum class Modifier
{
    None       = 0,
    LeftAlt    = 0x01,
    RightAlt   = 0x02,
    LeftCtrl   = 0x04,
    RightCtrl  = 0x08,
    LeftShift  = 0x10,
    RightShift = 0x20,
    LeftMeta   = 0x40,
    RightMeta  = 0x80
};

enum class Key
{
    None = 0,
    Esc,
    Return,
    Tab,
    Space,
    Backspace,
    Up,
    Down,
    Left,
    Right,
    Insert,
    Delete,
    Home,
    End,
    PageUp,
    PageDown,
    Print,
    Plus,
    Minus,
    LeftBracket,
    RightBracket,
    Semicolon,
    Quote,
    Comma,
    Period,
    Slash,
    Backslash,
    Tilde,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    NumPad0,
    NumPad1,
    NumPad2,
    NumPad3,
    NumPad4,
    NumPad5,
    NumPad6,
    NumPad7,
    NumPad8,
    NumPad9,
    Key0,
    Key1,
    Key2,
    Key3,
    Key4,
    Key5,
    Key6,
    Key7,
    Key8,
    Key9,
    KeyA,
    KeyB,
    KeyC,
    KeyD,
    KeyE,
    KeyF,
    KeyG,
    KeyH,
    KeyI,
    KeyJ,
    KeyK,
    KeyL,
    KeyM,
    KeyN,
    KeyO,
    KeyP,
    KeyQ,
    KeyR,
    KeyS,
    KeyT,
    KeyU,
    KeyV,
    KeyW,
    KeyX,
    KeyY,
    KeyZ,

    GamepadA,
    GamepadB,
    GamepadX,
    GamepadY,
    GamepadThumbL,
    GamepadThumbR,
    GamepadShoulderL,
    GamepadShoulderR,
    GamepadUp,
    GamepadDown,
    GamepadLeft,
    GamepadRight,
    GamepadBack,
    GamepadStart,
    GamepadGuide,

    Count
};

class DW_API Input : public Object {
public:
    DW_OBJECT(Input);

    Input(Context* context);
    ~Input();

    /// Toggles the relative mouse mode, which fixes the position and hides
    /// the cursor
    /// @param visible True if relative, false otherwise
    void lockCursor(bool relative);

    // Check the current state of the input devices
    bool isKeyDown(Key key) const;
    bool isMouseButtonDown(uint button) const;
    Vec2i getMousePosition() const;
    Vec2 getMousePositionRel() const;
    Vec3i getMouseMove() const;

private:
    Vec2i mViewportSize;
};

class DW_API EvtData_KeyDown : public EventData {
public:
    static const EventType eventType;

    EvtData_KeyDown(Key k, u16 m) : key(k), mod(m) {
    }
    const EventType& getType() const override {
        return eventType;
    }
    const String getName() const override {
        return "EvtData_KeyDown";
    }

    Key key;
    u16 mod;
};

class DW_API EvtData_KeyUp : public EventData {
public:
    static const EventType eventType;

    EvtData_KeyUp(Key k, u16 m) : key(k), mod(m) {
    }
    const EventType& getType() const override {
        return eventType;
    }
    const String getName() const override {
        return "EvtData_KeyDown";
    }

    Key key;
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
