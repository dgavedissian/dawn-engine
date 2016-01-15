/*
    Dawn Engine
    Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
    Author: David Avedissian
*/
#include "Common.h"
#include "Input.h"

NAMESPACE_BEGIN

const EventType EvtData_KeyDown::eventType(0xe135f7e7);
const EventType EvtData_KeyUp::eventType(0x3d00cddc);
const EventType EvtData_TextInput::eventType(0x4d82f23e);
const EventType EvtData_MouseDown::eventType(0x6f510a5e);
const EventType EvtData_MouseUp::eventType(0x2c080377);
const EventType EvtData_MouseMove::eventType(0xcfcf6020);
const EventType EvtData_MouseWheel::eventType(0xabc23f35);

Input::Input() : mInputBlock(0)
{
}

Input::~Input()
{
}

void Input::SetViewportSize(const Vec2i& viewportSize)
{
    mViewportSize = viewportSize;
}

void Input::LockCursor(bool relative)
{
    SDL_SetRelativeMouseMode((SDL_bool)relative);
}

void Input::PushInputBlock()
{
    mInputBlock++;
}

void Input::PopInputBlock()
{
    if (mInputBlock > 0)
        mInputBlock--;
}

bool Input::IsInputBlocked() const
{
    return mInputBlock > 0;
}

void Input::HandleSDLEvent(SDL_Event& e)
{
    switch (e.type)
    {
    case SDL_KEYDOWN:
        EventSystem::inst().QueueEvent(MakeShared<EvtData_KeyDown>(
            e.key.keysym.sym, e.key.keysym.scancode, e.key.keysym.mod));
        break;

    case SDL_KEYUP:
        EventSystem::inst().QueueEvent(
            MakeShared<EvtData_KeyUp>(e.key.keysym.sym, e.key.keysym.scancode, e.key.keysym.mod));
        break;

    case SDL_TEXTINPUT:
        EventSystem::inst().QueueEvent(MakeShared<EvtData_TextInput>(String(e.text.text)));
        break;

    case SDL_MOUSEBUTTONDOWN:
        EventSystem::inst().QueueEvent(MakeShared<EvtData_MouseDown>(e.button.button));
        break;

    case SDL_MOUSEBUTTONUP:
        EventSystem::inst().QueueEvent(MakeShared<EvtData_MouseUp>(e.button.button));
        break;

    case SDL_MOUSEMOTION:
        {
            Vec2i pos(e.motion.x, e.motion.y);
            Vec2 posRel(pos.x / (float)mViewportSize.x, pos.y / (float)mViewportSize.y);
            EventSystem::inst().QueueEvent(MakeShared<EvtData_MouseMove>(
                pos, posRel, Vec2i(e.motion.xrel, e.motion.yrel)));
        }
        break;

    case SDL_MOUSEWHEEL:
        EventSystem::inst().QueueEvent(
            MakeShared<EvtData_MouseWheel>(Vec2i(e.wheel.x, e.wheel.y)));
        break;

    default:
        break;
    }
}

bool Input::IsKeyDown(SDL_Keycode key) const
{
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    return (bool)state[SDL_GetScancodeFromKey(key)];
}

bool Input::IsMouseButtonDown(uint button) const
{
    return (bool)(SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(button));
}

Vec2i Input::GetMousePosition() const
{
    Vec2i pos;
    SDL_GetMouseState(&pos.x, &pos.y);
    return pos;
}

Vec2 Input::GetMousePositionRelative() const
{
    Vec2i mousePosition = GetMousePosition();
    return Vec2((float)mousePosition.x / mViewportSize.x, (float)mousePosition.y / mViewportSize.y);
}

Vec3i Input::GetMouseMove() const
{
    return Vec3i();
}

NAMESPACE_END
