/*
    Dawn Engine
    Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
    Author: David Avedissian
*/
#include "Common.h"
#include "Input.h"

namespace dw {

const EventType EvtData_Key::eventType(0x3d00cddc);
const EventType EvtData_CharInput::eventType(0x4d82f23e);
const EventType EvtData_MouseButton::eventType(0x2c080377);
const EventType EvtData_MouseMove::eventType(0xcfcf6020);
const EventType EvtData_MouseScroll::eventType(0xabc23f35);

Input::Input(Context* context)
    : Object(context),
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
    log().debug("Key %d state: %d - modifier: %d", key, state, modifier);
    triggerEvent<EvtData_Key>(key, modifier, state);
}

void Input::_notifyCharInput(const String& text) {
    triggerEvent<EvtData_CharInput>(text);
}

void Input::_notifyMouseButtonPress(MouseButton::Enum button, bool state) {
    mouse_button_state_[button] = state;
    triggerEvent<EvtData_MouseButton>(button, state);
}

void Input::_notifyMouseMove(const Vec2i& position) {
    mouse_move_ = position - mouse_position_;
    mouse_position_ = position;
    triggerEvent<EvtData_MouseMove>(mouse_position_, mousePositionRelative(), mouse_move_);
}

void Input::_notifyScroll(const Vec2& offset) {
    mouse_scroll_ = offset;
    triggerEvent<EvtData_MouseScroll>(offset);
}
}  // namespace dw
