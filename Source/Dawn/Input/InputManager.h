/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

NAMESPACE_BEGIN

class DW_API InputManager
{
public:
    InputManager();
    ~InputManager();

    /// Set the viewport size
    /// @param viewportSize Size of the viewport in pixels
    void SetViewportSize(const Vec2i& viewportSize);

    /// Toggles the relative mouse mode, which fixes the position and hides
    /// the cursor
    /// @param visible True if relative, false otherwise
    void LockCursor(bool relative);

    // Input blocking
    DEPRECATED void PushInputBlock();
    DEPRECATED void PopInputBlock();
    DEPRECATED bool IsInputBlocked() const;

    /// Process an SDL event. This should be called only by RenderSystem
    /// @param e SDL event structure
    void HandleSDLEvent(SDL_Event& e);

    // Check the current state of the input devices
    bool IsKeyDown(SDL_Keycode key) const;
    bool IsMouseButtonDown(uint button) const;
    Vec2i GetMousePosition() const;
    Vec2 GetMousePositionRelative() const;
    Vec3i GetMouseMove() const;

private:
    uint mInputBlock;
    Vec2i mViewportSize;

};



class DW_API EvtData_KeyDown : public EventData
{
public:
    static const EventType eventType;

    EvtData_KeyDown(SDL_Keycode kc, SDL_Scancode sc, u16 m) : keycode(kc), scancode(sc), mod(m)
    {
    }
    const EventType& GetEventType() const override
    {
        return eventType;
    }
    const string GetName() const override
    {
        return "EvtData_KeyDown";
    }

    SDL_Keycode keycode;
    SDL_Scancode scancode;
    u16 mod;
};

class DW_API EvtData_KeyUp : public EventData
{
public:
    static const EventType eventType;

    EvtData_KeyUp(SDL_Keycode kc, SDL_Scancode sc, u16 m) : keycode(kc), scancode(sc), mod(m)
    {
    }
    const EventType& GetEventType() const override
    {
        return eventType;
    }
    const string GetName() const override
    {
        return "EvtData_KeyUp";
    }

    SDL_Keycode keycode;
    SDL_Scancode scancode;
    u16 mod;
};

class DW_API EvtData_TextInput : public EventData
{
public:
    static const EventType eventType;

    EvtData_TextInput(string t) : text(t)
    {
    }
    const EventType& GetEventType() const override
    {
        return eventType;
    }
    const string GetName() const override
    {
        return "EvtData_TextInput";
    }

    string text;
};

class DW_API EvtData_MouseDown : public EventData
{
public:
    static const EventType eventType;

    EvtData_MouseDown(uint b) : button(b)
    {
    }
    const EventType& GetEventType() const override
    {
        return eventType;
    }
    const string GetName() const override
    {
        return "EvtData_MouseDown";
    }

    uint button;
};

class DW_API EvtData_MouseUp : public EventData
{
public:
    static const EventType eventType;

    EvtData_MouseUp(uint b) : button(b)
    {
    }
    const EventType& GetEventType() const override
    {
        return eventType;
    }
    const string GetName() const override
    {
        return "EvtData_MouseUp";
    }

    uint button;
};

class DW_API EvtData_MouseMove : public EventData
{
public:
    static const EventType eventType;

    EvtData_MouseMove(const Vec2i& p, const Vec2& pr, const Vec2i& m) : pos(p), posRel(pr), motion(m) {}
    const EventType& GetEventType() const override { return eventType; }
    const string GetName() const override { return "EvtData_MouseMove"; }

    Vec2i pos;
    Vec2 posRel;
    Vec2i motion;
};

class DW_API EvtData_MouseWheel : public EventData
{
public:
    static const EventType eventType;

    EvtData_MouseWheel(const Vec2i& m) : motion(m)
    {
    }
    const EventType& GetEventType() const override
    {
        return eventType;
    }
    const string GetName() const override
    {
        return "EvtData_MouseWheel";
    }

    Vec2i motion;
};

NAMESPACE_END
