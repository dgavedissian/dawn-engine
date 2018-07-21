/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "core/EventData.h"
#include "core/Preprocessor.h"

namespace dw {
#define EVENTSYSTEM_NUM_QUEUES 2

template <typename T, typename E>
EventDelegate makeEventDelegate(T* ptr, void (T::*handler)(const E&)) {
    return EventDelegate(ptr, handler);
}

class DW_API EventSystem : public Module {
public:
    DW_OBJECT(EventSystem);

    EventSystem(Context* context);
    virtual ~EventSystem();

    // Registers a delegate function that will get called when the event type is triggered
    bool addListener(const EventDelegate& event_delegate, const EventType& type);

    // Removes a delegate / event type pairing from the internal tables. Returns false if the
    // pairing was not found
    bool removeListener(const EventDelegate& event_delegate, const EventType& type);

    // Removes all delegate / event type pairings from a given delegate
    void removeAllListeners(const EventDelegate& event_delegate);

    // Fire off event NOW. This bypasses the queue entirely and immediately calls all delegate
    // functions registered for the event
    bool triggerEvent(const EventDataPtr& event_data) const;

    // Fire off event. This uses the queue and will call the delegate function on the next call to
    // update(), assuming there's enough time.
    bool queueEvent(const EventDataPtr& event_data);

    // Find the next-available instance of the named event type and remove it from the processing
    // queue. This may be done up to the point that it is actively being processed, eg. is safe to
    // happen during event processing itself.
    //
    // if allOfType is true, then all events of that type are cleared from the
    // input queue.
    //
    // returns true if the event was found and removed, false otherwise
    bool abortEvent(const EventType& type, bool all_of_type = false);

    // Allow for processing of any queued messages, optionally specify a processing time limit so
    // that the event processing does not take too long. Note the danger of using this artificial
    // limiter is that all messages may not in fact get processed.
    //
    // returns true if all messages ready for processing were completed, false
    // otherwise (e.g. timeout)
    bool update(double max_duration);

private:
    Map<EventType, List<EventDelegate>> event_listeners_;
    List<EventDataPtr> queues_[EVENTSYSTEM_NUM_QUEUES];

    // Index of actively processing queue; events enque to the opposing queue
    int active_queue_;

    // This flag is used to prevent add/removeListener from changing the eventListener List whilst
    // events are being processed as this causes a crash
    bool processing_events_;
    Map<EventType, List<EventDelegate>> added_event_listeners_;
    Map<EventType, List<EventDelegate>> remove_event_listeners_;
};

DEFINE_EMPTY_EVENT(ExitEvent);
DEFINE_EVENT(SendMessageEvent, String, sender, String, message);
DEFINE_EVENT(MessageEvent, String, sender, String, message);
}  // namespace dw
