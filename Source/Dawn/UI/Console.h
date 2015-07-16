/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

NAMESPACE_BEGIN

class UI;
class LuaState;
class EvtData_KeyDown;

// An object that sits in the top half of the screen
class DW_API Console : public LogListener, public Rocket::Core::EventListener
{
public:
    Console(UI* im, LuaState* ls);
    virtual ~Console();

    // Set the visibility of the console
    void SetVisible(bool visible);

    // Returns the state of the visibility flag
    bool IsVisible() const;

    // Internal: Write a line
    void Write(const String& str);

    // Internal: Execute a lua statement
    void Execute(const String& statement);

    // Inherited from LogListener
    virtual void LogWrite(const String& msg) override;

    // Inherited from Rocket::Core::EventListener
    virtual void ProcessEvent(Rocket::Core::Event& event) override;

private:
    UI* mInterfaceMgr;
    LuaState* mLuaState;

    Layout* mLayout;
    Rocket::Core::Element* mConsole;
    Rocket::Core::Element* mText;

    String mOutput;
};

NAMESPACE_END
