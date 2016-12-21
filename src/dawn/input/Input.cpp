/*
    Dawn Engine
    Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
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

void Input::lockCursor(bool relative) {
}

bool Input::isKeyDown(Key key) const {
    return false;
}

bool Input::isMouseButtonDown(uint button) const {
    return false;
}

Vec2i Input::getMousePosition() const {
    Vec2i pos;
    return pos;
}

Vec2 Input::getMousePositionRel() const {
    Vec2i mousePosition = getMousePosition();
    return Vec2((float)mousePosition.x / mViewportSize.x, (float)mousePosition.y / mViewportSize.y);
}

Vec3i Input::getMouseMove() const {
    return Vec3i();
}
}
