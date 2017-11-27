/*
    Dawn Engine
    Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
    Author: David Avedissian
*/
#include "Common.h"
#include "Input.h"

namespace dw {
Input::Input(Context* context)
    : Subsystem(context),
      viewport_size_(1280, 800),
      mouse_position_(0, 0),
      mouse_move_(0, 0),
      mouse_scroll_(0.0f, 0.0f) {
    for (bool& i : key_down_) {
        i = false;
    }
    for (bool& i : mouse_button_state_) {
        i = false;
    }
}

Input::~Input() {
}

bool Input::isKeyDown(Key::Enum key) const {
    return key_down_[key];
}

bool Input::isMouseButtonDown(MouseButton::Enum button) const {
    return mouse_button_state_[button];
}

Vec2i Input::mousePosition() const {
    return mouse_position_;
}

Vec2 Input::mousePositionRelative() const {
    Vec2i mouse_position = mousePosition();
    return {static_cast<float>(mouse_position.x) / viewport_size_.x,
            static_cast<float>(mouse_position.y) / viewport_size_.y};
}

Vec2i Input::mouseMove() const {
    return mouse_move_;
}

Vec2 Input::mouseScroll() const {
    return mouse_scroll_;
}

void Input::_notifyKey(Key::Enum key, Modifier::Enum modifier, bool state) {
    key_down_[key] = state;
    triggerEvent<KeyEvent>(key, modifier, state);
}

void Input::_notifyCharInput(const String& text) {
    triggerEvent<CharInputEvent>(text);
}

void Input::_notifyMouseButtonPress(MouseButton::Enum button, bool state) {
    mouse_button_state_[button] = state;
    triggerEvent<MouseButtonEvent>(button, state);
}

void Input::_notifyMouseMove(const Vec2i& position) {
    mouse_move_ = position - mouse_position_;
    mouse_position_ = position;
    triggerEvent<MouseMoveEvent>(mouse_position_, mousePositionRelative(), mouse_move_);
}

void Input::_notifyScroll(const Vec2& offset) {
    mouse_scroll_ = offset;
    triggerEvent<MouseScrollEvent>(offset);
}
}  // namespace dw
