/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "core/math/Vec2i.h"
#include "core/math/Vec3i.h"
#include "core/EventSystem.h"
#include <dawn-gfx/Input.h>

namespace dw {
namespace Modifier {
enum Enum {
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
}

namespace Key {
enum Enum {
    // Printable keys.
    Space = 0,
    Apostrophe, /* ' */
    Comma,      /* , */
    Minus,      /* - */
    Period,     /* . */
    Slash,      /* / */
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
    Semicolon, /* ; */
    Equal,     /* = */
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    LeftBracket,  /* [ */
    Backslash,    /* \ */
    RightBracket, /* ] */
    Backtick,     /* ` */

    // Function keys.
    Escape,
    Enter,
    Tab,
    Backspace,
    Insert,
    Delete,
    Right,
    Left,
    Down,
    Up,
    PageUp,
    PageDown,
    Home,
    End,
    CapsLock,
    ScrollLock,
    NumLock,
    PrintScreen,
    Pause,
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
    KeyPadDecimal,
    KPDivide,
    KPMultiply,
    KPSubtract,
    KPAdd,
    KPEnter,
    KPEqual,
    LeftShift,
    LeftCtrl,
    LeftAlt,
    LeftSuper,
    RightShift,
    RightCtrl,
    RightAlt,
    RightSuper,

    Count
};
}

namespace MouseButton {
enum Enum { Left = 0, Middle, Right, Count };
}

class DW_API Input : public Module {
public:
    DW_OBJECT(Input);

    Input(Context* context);
    ~Input();

    /// Register with event system.
    void registerEventSystem(EventSystem* event_system);
    void unregisterEventSystem(EventSystem* event_system);

    /// Key state.
    bool isKeyDown(Key::Enum key) const;

    /// Mouse button state.
    bool isMouseButtonDown(MouseButton::Enum button) const;

    /// Mouse position.
    Vec2i mousePosition() const;

    /// Mouse position (relative to viewport size).
    Vec2 mousePositionRelative() const;

    /// Mouse move since last event.
    Vec2i mouseMove() const;

    /// Mouse scroll since last event.
    Vec2 mouseScroll() const;

    // Generate input callbacks struct for dawn-gfx.
    gfx::InputCallbacks getGfxInputCallbacks();

private:
    Vec2i viewport_size_;
    bool key_down_[Key::Count];
    bool mouse_button_state_[MouseButton::Count];
    Vec2i mouse_position_;
    Vec2i mouse_move_;
    Vec2 mouse_scroll_;

    Vector<EventSystem*> event_systems_;

    void onKey(Key::Enum key, Modifier::Enum modifier, bool pressed);
    void onCharInput(const String& text);
    void onMouseButton(MouseButton::Enum button, bool pressed);
    void onMouseMove(const Vec2i& position);
    void onMouseScroll(const Vec2& offset);
};

DEFINE_EVENT(KeyEvent, Key::Enum, key, Modifier::Enum, mod, bool, down);
DEFINE_EVENT(CharInputEvent, String, text);  // Text is a single UTF-8 character
DEFINE_EVENT(MouseButtonEvent, MouseButton::Enum, button, bool, down);
DEFINE_EVENT(MouseMoveEvent, Vec2i, position, Vec2, position_relative, Vec2i, offset);
DEFINE_EVENT(MouseScrollEvent, Vec2, motion);
}  // namespace dw
