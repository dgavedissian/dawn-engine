/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#include "Common.h"
#include "UI.h"
#include "Layout.h"

NAMESPACE_BEGIN

Layout::Layout(UI* im, Rocket::Core::ElementDocument* document)
    : mInterfaceMgr(im),
      mDocument(document)
{
    document->Show();
}

Layout::~Layout()
{
    // This was a tricky bug to fix.. When you remove a reference in
    // libRocket, it doesn't actually delete anything until
    // mContext->RemoveReference. This causes it to call Listener::OnDelete()
    // after the state object has been deleted, so lets clean up here...
    for (auto& t : mListeners)
    {
        mDocument->GetElementById(std::get<0>(t).c_str())
            ->RemoveEventListener(std::get<1>(t).c_str(), std::get<2>(t));
    }

    mDocument->RemoveReference();
    mDocument->Close();
}

void Layout::FocusOn(const string& id)
{
    if (!mDocument->GetElementById(id.c_str())->Focus())
    {
        assert(true && "Unable to attain focus");
    }
}

void Layout::Show(int showType)
{
    mDocument->Show(showType);
}

void Layout::Hide()
{
    // Disable modality by calling "Show" again
    if (mDocument->IsModal())
        mDocument->Show(Rocket::Core::ElementDocument::NONE);

    // Remove focus
    mDocument->GetFocusLeafNode()->Blur();

    // Hide
    mDocument->Hide();
}

bool Layout::IsVisible() const
{
    return mDocument->IsVisible();
}

void Layout::BindEvent(const string& id, UIEvent event)
{
    // Map event ID to string
    string eventID;
    switch (event)
    {
    case UI_CLICK:
        eventID = "click";
        break;

    case UI_SUBMIT:
        eventID = "submit";
        break;

    default:
        break;
    }
    
    // Look up the element
    Rocket::Core::Element* elem = mDocument->GetElementById(id.c_str());
    assert(elem);
    
    // Add the listener
    elem->AddEventListener(eventID.c_str(), mInterfaceMgr);
    mListeners.push_back(make_tuple(id, eventID, mInterfaceMgr));
}

Rocket::Core::Element* Layout::GetElementById(Rocket::Core::String id)
{
    return mDocument->GetElementById(id);
}

Rocket::Core::ElementDocument* Layout::GetDocument()
{
    return mDocument;
}

NAMESPACE_END
