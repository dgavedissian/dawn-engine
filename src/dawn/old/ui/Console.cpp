/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "core/Utils.h"
#include "script/LuaState.h"
#include "ui/Console.h"
#include "ui/UI.h"

namespace dw {

Console::Console(UI* im, LuaState* ls)
    : mInterfaceMgr(im), mLuaState(ls), mLayout(nullptr), mConsole(nullptr), mText(nullptr) {
    mLayout = mInterfaceMgr->loadLayout("console.rml");
    mLayout->hide();

    // Bit of a hack, bypass event system and hook this class directly
    mLayout->getElementById("cmd")->AddEventListener("keydown", this);
    mConsole = mLayout->getElementById("console");
    mText = mLayout->getElementById("consoletext");

    // Copy all the lines from the log buffer
    auto lines = Log::inst().getBuffer();
    for (auto i = lines.begin(); i != lines.end(); ++i)
        write(*i);
}

Console::~Console() {
    mLayout->getElementById("cmd")->RemoveEventListener("keydown", this);
    mInterfaceMgr->unloadLayout(mLayout);
}

void Console::setVisible(bool visible) {
    if (visible) {
        mLayout->show(Modal);
    } else {
        mLayout->hide();
    }
}

bool Console::isVisible() const {
    return mLayout->isVisible();
}

void Console::write(const String& str) {
    // Add a new line if not empty
    if (!mOutput.empty()) {
        mOutput += "\n";
    }

    // Sanitise the input
    String sanitisedStr(str);
    sanitisedStr = replaceString(sanitisedStr, "<", "&lt;");
    sanitisedStr = replaceString(sanitisedStr, ">", "&gt;");

    // Add the string
    mOutput += sanitisedStr;

    // Set the inner RML and scroll down
    if (mText) {
        mText->SetInnerRML(mOutput.c_str());
        mConsole->SetScrollTop(mConsole->GetScrollHeight());
    }
}

void Console::execute(const String& statement) {
    write("> " + statement);
    mLuaState->executeString(statement);
}

void Console::logWrite(const String& msg) {
    write(msg);
}

void Console::ProcessEvent(Rocket::Core::Event& event) {
    String controlId(event.GetCurrentElement()->GetId().CString());
    if (controlId == "cmd") {
        int key = event.GetParameters()->Get<int>("key_identifier", 0);
        Rocket::Controls::ElementFormControl* inputBox =
            static_cast<Rocket::Controls::ElementFormControl*>(event.GetCurrentElement());
        if (key == Rocket::Core::Input::KI_RETURN) {
            String command = inputBox->GetValue().CString();
            inputBox->SetValue("");
            execute(command);
        }
    }
}
}
