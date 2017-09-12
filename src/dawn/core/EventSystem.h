/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "core/EventData.h"
#include "core/Preprocessor.h"

// Macros to make adding/removing event Listeners more sane
#define ADD_LISTENER(LISTENER, EVENT) \
    addEventListener<EVENT>(makeDelegate(this, &LISTENER::handleEvent))
#define REMOVE_LISTENER(LISTENER, EVENT) \
    removeEventListener<EVENT>(makeDelegate(this, &LISTENER::handleEvent))

// Event definition generator macro.
#define DEFINE_EMPTY_EVENT(name)                        \
    struct name : public dw::EventData {                \
        static const dw::EventType eventType;           \
        name() {                                        \
        }                                               \
        const dw::EventType& getType() const override { \
            return eventType;                           \
        }                                               \
        dw::String getName() const override {           \
            return #name;                               \
        }                                               \
    }
#define DEFINE_EVENT(name, ...)                                                    \
    struct name : public dw::EventData {                                           \
        static const dw::EventType eventType;                                      \
        name(EVENT_CTOR_ARGS(__VA_ARGS__)) : EVENT_INITIALISER_LIST(__VA_ARGS__) { \
        }                                                                          \
        const dw::EventType& getType() const override {                            \
            return eventType;                                                      \
        }                                                                          \
        dw::String getName() const override {                                      \
            return #name;                                                          \
        }                                                                          \
        EVENT_DATA_FIELDS(__VA_ARGS__)                                             \
    }
#define EVENT_CTOR_ARGS(...) CALL_OVERLOAD(EVENT_CTOR_ARGS, __VA_ARGS__)
#define EVENT_CTOR_ARGS2(type, name) const type1& name1
#define EVENT_CTOR_ARGS4(type1, name1, type2, name2) const type1 &name1, const type2 &name2
#define EVENT_CTOR_ARGS6(type1, name1, type2, name2, type3, name3) \
    const type1 &name1, const type2 &name2, const type3 &name3
#define EVENT_INITIALISER_LIST(...) CALL_OVERLOAD(EVENT_INITIALISER_LIST, __VA_ARGS__)
#define EVENT_INITIALISER_LIST2(type, name) name1(name1)
#define EVENT_INITIALISER_LIST4(type1, name1, type2, name2) name1(name1), name2(name2)
#define EVENT_INITIALISER_LIST6(type1, name1, type2, name2, type3, name3) \
    name1(name1), name2(name2), name3(name3)
#define EVENT_DATA_FIELDS(...) CALL_OVERLOAD(EVENT_DATA_FIELDS, __VA_ARGS__)
#define EVENT_DATA_FIELDS2(type1, name1) type1 name1;
#define EVENT_DATA_FIELDS4(type1, name1, type2, name2) \
    type1 name1;                                       \
    type2 name2;
#define EVENT_DATA_FIELDS6(type1, name1, type2, name2, type3, name3) \
    type1 name1;                                                     \
    type2 name2;                                                     \
    type3 name3;

namespace dw {
#define EVENTSYSTEM_NUM_QUEUES 2

template <class T> bool eventIs(const EventDataPtr eventData) {
    return eventData->getType() == T::eventType;
}

template <class T> SharedPtr<T> castEvent(const EventDataPtr eventData) {
    return staticPointerCast<T>(eventData);
}

class DW_API EventSystem : public Object {
public:
    DW_OBJECT(EventSystem);

    EventSystem(Context* context);
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

DEFINE_EMPTY_EVENT(ExitEvent);
DEFINE_EVENT(SendMessageEvent, String, sender, String, message);
DEFINE_EVENT(MessageEvent, String, sender, String, message);
}  // namespace dw
