/*
    Dawn Engine
    Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
    Author: David Avedissian
*/
#include "Common.h"
#include "Input.h"

namespace dw {

const EventType EvtData_KeyDown::eventType(0xe135f7e7);
const EventType EvtData_KeyUp::eventType(0x3d00cddc);
const EventType EvtData_TextInput::eventType(0x4d82f23e);
const EventType EvtData_MouseDown::eventType(0x6f510a5e);
const EventType EvtData_MouseUp::eventType(0x2c080377);
const EventType EvtData_MouseMove::eventType(0xcfcf6020);
const EventType EvtData_MouseWheel::eventType(0xabc23f35);

Input::Input(Context* context) : Object(context) {
}

Input::~Input() {
}

bool Input::isKeyDown(Key key) const {
    return keyboard_state_[(int)key];
}

bool Input::isMouseButtonDown(MouseButton button) const {
    return mouse_button_state_[(int)button];
}

Vec2i Input::mousePosition() const {
    return mouse_position_;
}

Vec2 Input::mousePositionRelative() const {
    Vec2i mouse_position = mousePosition();
    return {(float)mouse_position.x / viewport_size_.x, (float)mouse_position.y / viewport_size_.y};
}

Vec2i Input::mouseMove() const {
    return mouse_move_;
}

Vec2 Input::mouseScroll() const {
    return mouse_scroll_;
}

void Input::_notifyKeyPress(Key key, Modifier modifier, bool state) {
    keyboard_state_[(int)key] = state;
    log().debug("Key %d state: %d - modifier: %d", (int)key, (int)state, (int)modifier);
    // TODO: Send event.
}

void Input::_notifyMouseButtonPress(MouseButton button, bool state) {
    mouse_button_state_[(int)button] = state;
    log().debug("Mouse Button %d state: %d", (int)button, (int)state);
    // TODO: Send event.
}

void Input::_notifyMouseMove(const Vec2i& position) {
    mouse_move_ = position - mouse_position_;
    mouse_position_ = position;
    log().debug("Mouse position {%d,%d}", position.x, position.y);
    // TODO: Send event.
}

void Input::_notifyScroll(const Vec2& offset) {
    mouse_scroll_ = offset;
    // TODO: Send event.
}
}  // namespace dw
