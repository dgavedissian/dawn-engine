/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

NAMESPACE_BEGIN

class UI;

enum ShowType
{
    None = Rocket::Core::ElementDocument::NONE,
    Focus = Rocket::Core::ElementDocument::FOCUS,
    Modal = Rocket::Core::ElementDocument::MODAL
};

enum UIEvent
{
    UI_MOUSE_ENTER,
    UI_MOUSE_LEAVE,
    UI_CLICK,
    UI_SUBMIT
};

class DW_API Layout
{
public:
    Layout(UI* im, Rocket::Core::ElementDocument* document);
    ~Layout();

    /// Focus on an element in this layout
    void FocusOn(const String& id);

    /// Show the layout
    void Show(int showType = Focus);

    /// Hide the layout
    void Hide();

    /// Get the current visibility of the layout
    /// @return true if the layout is visible, false otherwise
    bool IsVisible() const;

    /// Add a new event Listener
    /// id    ID of the element to bind to
    /// event Event identifier
    void BindEvent(const String& id, UIEvent event);

    // Internal: Get element by Id
    Rocket::Core::Element* GetElementById(Rocket::Core::String id);

    // Internal: Get built in document
    DEPRECATED Rocket::Core::ElementDocument* GetDocument();

private:
    UI* mInterfaceMgr;

    Rocket::Core::ElementDocument* mDocument;
    List<std::tuple<String, String, Rocket::Core::EventListener*>> mListeners;
};

NAMESPACE_END
