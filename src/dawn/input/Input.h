/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "math/Vec2i.h"
#include "math/Vec3i.h"

namespace dw {

enum class Modifier : u16 {
    None = 0,
    LeftAlt = 0x01,
    RightAlt = 0x02,
    LeftCtrl = 0x04,
    RightCtrl = 0x08,
    LeftShift = 0x10,
    RightShift = 0x20,
    LeftMeta = 0x40,
    RightMeta = 0x80
};

enum class Key {
    Esc = 0,
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
    Plus,         /* + */
    Minus,        /* - */
    Equals,       /* = */
    LeftBracket,  /* [ */
    RightBracket, /* ] */
    Semicolon,    /* ; */
    Quote,        /* " */
    Apostrophe,   /* ' */
    Comma,        /* , */
    Period,       /* . */
    Slash,        /* / */
    Backslash,    /* \ */
    Tilde,        /* ~ */
    Backtick,     /* ` */
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

enum class MouseButton { Left = 0, Middle, Right, Count };

class DW_API Input : public Object {
public:
    DW_OBJECT(Input);

    Input(Context* context);
    ~Input();

    /// Key state.
    bool isKeyDown(Key key) const;

    /// Mouse button state.
    bool isMouseButtonDown(MouseButton button) const;

    /// Mouse position.
    Vec2i mousePosition() const;

    /// Mouse position (relative to viewport size).
    Vec2 mousePositionRelative() const;

    /// Mouse move since last event.
    Vec2i mouseMove() const;

    /// Mouse scroll since last event.
    Vec2 mouseScroll() const;

    // Notifications. Internal.
    void _notifyKeyPress(Key key, Modifier modifier, bool state);
    void _notifyMouseButtonPress(MouseButton button, bool state);
    void _notifyMouseMove(const Vec2i& position);
    void _notifyScroll(const Vec2& offset);

private:
    Vec2i viewport_size_;
    bool keyboard_state_[(int)Key::Count];
    bool mouse_button_state_[(int)MouseButton::Count];
    Vec2i mouse_position_;
    Vec2i mouse_move_;
    Vec2 mouse_scroll_;
};

class DW_API EvtData_KeyDown : public EventData {
public:
    static const EventType eventType;

    EvtData_KeyDown(Key k, u16 m) : key(k), mod(m) {
    }

    const EventType& getType() const override {
        return eventType;
    }

    String getName() const override {
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

    String getName() const override {
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

    String getName() const override {
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

    String getName() const override {
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

    String getName() const override {
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

    String getName() const override {
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

    String getName() const override {
        return "EvtData_MouseWheel";
    }

    Vec2i motion;
};
}  // namespace dw
