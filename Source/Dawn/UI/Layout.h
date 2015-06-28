/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

NAMESPACE_BEGIN

class InterfaceManager;

enum ShowType
{
    None = Rocket::Core::ElementDocument::NONE,
    Focus = Rocket::Core::ElementDocument::FOCUS,
    Modal = Rocket::Core::ElementDocument::MODAL
};

enum UIEvent
{
    UI_CLICK,
    UI_SUBMIT
};

class DW_API Layout
{
public:
    Layout(InterfaceManager* im, Rocket::Core::ElementDocument* document);
    ~Layout();

    /// Focus on an element in this layout
    void FocusOn(const string& id);

    /// Show the layout
    void Show(int showType = Focus);

    /// Hide the layout
    void Hide();

    /// Get the current visibility of the layout
    /// @return true if the layout is visible, false otherwise
    bool IsVisible() const;

    /// Add a new event listener
    /// id    ID of the element to bind to
    /// event Event identifier
    void BindEvent(const string& id, UIEvent event);

    // Internal: Get element by Id
    Rocket::Core::Element* GetElementById(Rocket::Core::String id);

    // Internal: Get built in document
    DEPRECATED Rocket::Core::ElementDocument* GetDocument();

private:
    InterfaceManager* mInterfaceMgr;

    Rocket::Core::ElementDocument* mDocument;
    std::list<std::tuple<string, string, Rocket::Core::EventListener*>> mListeners;
};

NAMESPACE_END
