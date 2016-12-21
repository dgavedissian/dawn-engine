/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

namespace dw {

class UI;
class LuaState;
class EvtData_KeyDown;
class Layout;

// An object that sits in the top half of the screen
class DW_API Console : public LogListener, public Rocket::Core::EventListener {
public:
    Console(UI* im, LuaState* ls);
    virtual ~Console();

    // Set the visibility of the console
    void setVisible(bool visible);

    // Returns the state of the visibility flag
    bool isVisible() const;

    // Internal: Write a line
    void write(const String& str);

    // Internal: Execute a lua statement
    void execute(const String& statement);

    // Inherited from LogListener
    virtual void logWrite(const String& msg) override;

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
}
