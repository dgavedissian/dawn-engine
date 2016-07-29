/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

// Macros to make adding/removing event Listeners more sane
#define ADD_LISTENER(LISTENER, EVENT) \
    dw::EventSystem::inst().addListener(fastdelegate::MakeDelegate(this, &LISTENER::handleEvent), \
                                    EVENT::eventType);
#define REMOVE_LISTENER(LISTENER, EVENT) \
    dw::EventSystem::inst().removeListener(fastdelegate::MakeDelegate(this, &LISTENER::handleEvent), \
                                       EVENT::eventType);
#define REMOVE_ALL_LISTENERS(LISTENER) \
    dw::EventSystem::inst().removeAllListeners(fastdelegate::MakeDelegate(this, &LISTENER::handleEvent))

NAMESPACE_BEGIN

// Event type ID
typedef uint EventType;

// Event data interface
class DW_API EventData
{
public:
    virtual ~EventData() {}
    virtual const EventType& getType() const = 0;
    virtual const String getName() const = 0;

    // Serialisation for network input/output
    virtual void serialise(std::ostream& out) const {}
    virtual void deserialise(std::istream& in) {}
};

typedef SharedPtr<EventData> EventDataPtr;
typedef fastdelegate::FastDelegate1<SharedPtr<EventData>> EventListenerDelegate;

// Event listener interface
class DW_API EventListener
{
    virtual void handleEvent(EventDataPtr eventData) = 0;
};

#define EVENTSYSTEM_NUM_QUEUES 2

template <class T> bool eventIs(const EventDataPtr eventData)
{
    return eventData->getType() == T::eventType;
}

template <class T> SharedPtr<T> castEvent(const EventDataPtr eventData)
{
    return staticPointerCast<T>(eventData);
}

class DW_API EventSystem : public Singleton<EventSystem>
{
public:
    EventSystem();
    virtual ~EventSystem();

    // Registers a delegate function that will get called when the event type is triggered
    bool addListener(const EventListenerDelegate& eventDelegate, const EventType& type);

    // Removes a delegate / event type pairing from the internal tables. Returns false if the
    // pairing was not found
    bool removeListener(const EventListenerDelegate& eventDelegate, const EventType& type);

    // Removes all delegate / event type pairings from a given delegate
    void removeAllListeners(const EventListenerDelegate& eventDelegate);

    // Fire off event NOW. This bypasses the queue entirely and immediately calls all delegate
    // functions registered for the event
    bool triggerEvent(const EventDataPtr& eventData) const;

    // Fire off event. This uses the queue and will call the delegate function on the next call to
    // update(), assuming there's enough time.
    bool queueEvent(const EventDataPtr& eventData);
    // bool threadSafeQueueEvent(const IEventDataPtr& pEvent);

    // Find the next-available instance of the named event type and remove it from the processing
    // queue. This may be done up to the point that it is actively being processed, eg. is safe to
    // happen during event processing itself.
    //
    // if allOfType is true, then all events of that type are cleared from the
    // input queue.
    //
    // returns true if the event was found and removed, false otherwise
    bool abortEvent(const EventType& type, bool allOfType = false);

    // Allow for processing of any queued messages, optionally specify a processing time limit so
    // that the event processing does not take too long. Note the danger of using this artificial
    // limiter is that all messages may not in fact get processed.
    //
    // returns true if all messages ready for processing were completed, false
    // otherwise (e.g. timeout)
    bool update(double maxDuration);

private:
    Map<EventType, List<EventListenerDelegate>> mEventListeners;
    List<EventDataPtr> mQueues[EVENTSYSTEM_NUM_QUEUES];

    // Index of actively processing queue; events enque to the opposing queue
    int mActiveQueue;

    // This flag is used to prevent add/removeListener from changing the eventListener List whilst
    // events are being processed as this causes a crash
    bool mProcessingEvents;
    Map<EventType, List<EventListenerDelegate>> mAddedEventListeners;
    Map<EventType, List<EventListenerDelegate>> mRemovedEventListeners;
};

// Some bog-standard events
class DW_API EvtData_Exit : public EventData
{
public:
    static const EventType eventType;

    EvtData_Exit() { }
    virtual const EventType& getType() const override { return eventType; }
    virtual const String getName() const override { return "EvtData_ExitEvent"; }
};

class DW_API EvtData_SendMessage : public EventData
{
public:
    static const EventType eventType;

    EvtData_SendMessage(const String& s, const String& m) : sender(s), message(m) {}
    virtual const EventType& getType() const override { return eventType; }
    virtual const String getName() const override { return "EvtData_SendMessage"; }

    String sender;
    String message;
};

class DW_API EvtData_Message : public EventData
{
public:
    static const EventType eventType;

    EvtData_Message(const String& s, const String& m) : sender(s), message(m) {}
    virtual const EventType& getType() const override { return eventType; }
    virtual const String getName() const override { return "EvtData_Message"; }

    String sender;
    String message;
};

NAMESPACE_END
