/*
    Dawn Engine
    Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
    Author: David Avedissian
*/
#include "Base.h"
#include "Input.h"

namespace dw {
Input::Input(Context* context)
    : Module(context),
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

void Input::registerEventSystem(EventSystem* event_system) {
    event_systems_.emplace_back(event_system);
}

void Input::unregisterEventSystem(EventSystem* event_system) {
    auto it = std::find(event_systems_.begin(), event_systems_.end(), event_system);
    if (it != event_systems_.end()) {
        event_systems_.erase(it);
    }
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
    for (auto es : event_systems_) {
        es->triggerEvent<KeyEvent>(key, modifier, state);
    }
}

void Input::_notifyCharInput(const String& text) {
    for (auto es : event_systems_) {
        es->triggerEvent<CharInputEvent>(text);
    }
}

void Input::_notifyMouseButtonPress(MouseButton::Enum button, bool state) {
    mouse_button_state_[button] = state;
    for (auto es : event_systems_) {
        es->triggerEvent<MouseButtonEvent>(button, state);
    }
}

void Input::_notifyMouseMove(const Vec2i& position) {
    mouse_move_ = position - mouse_position_;
    mouse_position_ = position;
    for (auto es : event_systems_) {
        es->triggerEvent<MouseMoveEvent>(mouse_position_, mousePositionRelative(), mouse_move_);
    }
}

void Input::_notifyScroll(const Vec2& offset) {
    mouse_scroll_ = offset;
    for (auto es : event_systems_) {
        es->triggerEvent<MouseScrollEvent>(offset);
    }
}
}  // namespace dw
