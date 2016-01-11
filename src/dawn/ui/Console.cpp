/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#include "Common.h"
#include "Script/LuaState.h"
#include "UI.h"
#include "Console.h"

NAMESPACE_BEGIN

Console::Console(UI* im, LuaState* ls)
    : mInterfaceMgr(im),
      mLuaState(ls),
      mLayout(nullptr),
      mConsole(nullptr),
      mText(nullptr)
{
    mLayout = mInterfaceMgr->LoadLayout("console.rml");
    mLayout->Hide();

    // Bit of a hack, bypass event system and hook this class directly
    mLayout->GetElementById("cmd")->AddEventListener("keydown", this);
    mConsole = mLayout->GetElementById("console");
    mText = mLayout->GetElementById("consoletext");

    // Copy all the lines from the log buffer
    auto lines = Log::inst().GetLogBuffer();
    for (auto i = lines.begin(); i != lines.end(); ++i)
        Write(*i);
}

Console::~Console()
{
    mLayout->GetElementById("cmd")->RemoveEventListener("keydown", this);
    mInterfaceMgr->UnloadLayout(mLayout);
}

void Console::SetVisible(bool visible)
{
    if (visible)
    {
        mLayout->Show(Modal);
    }
    else
    {
        mLayout->Hide();
    }
}

bool Console::IsVisible() const
{
    return mLayout->IsVisible();
}

void Console::Write(const String& str)
{
    // Add a new line if not empty
    if (!mOutput.empty())
    {
        mOutput += "\n";
    }

    // Sanitise the input
    String sanitisedStr(str);
    sanitisedStr = Replace(sanitisedStr, "<", "&lt;");
    sanitisedStr = Replace(sanitisedStr, ">", "&gt;");

    // Add the string
    mOutput += sanitisedStr;

    // Set the inner RML and scroll down
	if (mText)
	{
		mText->SetInnerRML(mOutput.c_str());
		mConsole->SetScrollTop(mConsole->GetScrollHeight());
	}
}

void Console::Execute(const String& statement)
{
    Write("> " + statement);
    mLuaState->ExecuteString(statement);
}

void Console::LogWrite(const String& msg)
{
    Write(msg);
}

void Console::ProcessEvent(Rocket::Core::Event& event)
{
    String controlId(event.GetCurrentElement()->GetId().CString());
    if (controlId == "cmd")
    {
        int key = event.GetParameters()->Get<int>("key_identifier", 0);
        Rocket::Controls::ElementFormControl* inputBox =
            static_cast<Rocket::Controls::ElementFormControl*>(event.GetCurrentElement());
        if (key == Rocket::Core::Input::KI_RETURN)
        {
            String command = inputBox->GetValue().CString();
            inputBox->SetValue("");
            Execute(command);
        }
    }
}

NAMESPACE_END
