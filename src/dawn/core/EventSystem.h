/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "core/EventData.h"

namespace dw {
#define EVENTSYSTEM_NUM_QUEUES 2

template <typename T, typename E>
EventDelegate makeEventDelegate(T* ptr, void (T::*handler)(const E&)) {
    return EventDelegate(ptr, handler);
}

class DW_API EventSystem : public Object {
public:
    DW_OBJECT(EventSystem);

    EventSystem(Context* context);
    virtual ~EventSystem();

    /// Registers a delegate function that will get called when the event type is triggered.
    template <typename E, typename T> bool addListener(T* listener, void (T::*handler)(const E&));

    /// Removes a delegate / event type pairing from the internal tables. Returns false if the
    /// pairing was not found.
    template <typename E, typename T>
    bool removeListener(T* listener, void (T::*handler)(const E&));

    // TODO: Remove all delegates for a given listener.

    /// Fire off event NOW. This bypasses the queue entirely and immediately calls all delegate
    /// functions registered for the event.
    template <typename T, typename... Args> bool triggerEvent(Args&&... args) const;

    /// Fire off event. This uses the queue and will call the delegate function on the next call to
    /// update(), assuming there's enough time.
    template <typename T, typename... Args> bool queueEvent(Args&&... args) const;

    /// Find the next-available instance of the named event type and remove it from the processing
    /// queue. This may be done up to the point that it is actively being processed, eg. is safe to
    /// happen during event processing itself.
    ///
    /// if allOfType is true, then all events of that type are cleared from the input queue.
    ///
    /// returns true if the event was found and removed, false otherwise.
    bool abortEvent(const EventType& type, bool all_of_type = false);

    /// Allow for processing of any queued messages, optionally specify a processing time limit so
    /// that the event processing does not take too long. Note the danger of using this artificial
    /// limiter is that all messages may not in fact get processed.
    ///
    /// returns true if all messages ready for processing were completed, false otherwise (e.g.
    /// timeout)
    bool update(double max_duration);

private:
    bool addDelegate(EventDelegate event_delegate, EventType type);
    bool removeDelegate(EventDelegate event_delegate, EventType type);

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

template <typename E, typename T>
bool EventSystem::addListener(T* listener, void (T::*handler)(const E&)) {
    EventDelegate event_delegate = makeEventDelegate<T, E>(listener, handler);
    return addDelegate(event_delegate, E::typeStatic());
}

template <typename E, typename T>
bool EventSystem::removeListener(T* listener, void (T::*handler)(const E&)) {
    EventDelegate event_delegate = makeEventDelegate<T, E>(listener, handler);
    return removeDelegate(event_delegate, E::typeStatic());
}

template <typename T, typename... Args> bool EventSystem::triggerEvent(Args&&... args) const {
    bool processed = false;

    auto event_data = makeShared<T>(std::forward<Args>(args)...);

    auto find_it = event_listeners_.find(event_data->type());
    if (find_it != event_listeners_.end()) {
        auto& listeners = find_it->second;
        for (const auto& delegate_function : listeners) {
            delegate_function(event_data);
            processed = true;
        }
    }

    return processed;
}

template <typename T, typename... Args> bool EventSystem::queueEvent(Args&&... args) const {
    assert(active_queue_ >= 0);
    assert(active_queue_ < EVENTSYSTEM_NUM_QUEUES);

    auto event_data = makeShared<T>(std::forward<Args>(args)...);

    auto find_it = event_listeners_.find(event_data->type());
    if (find_it != event_listeners_.end()) {
        queues_[active_queue_].push_back(event_data);
        return true;
    }
    return false;
}

DEFINE_EMPTY_EVENT(ExitEvent);
DEFINE_EVENT(SendMessageEvent, String, sender, String, message);
DEFINE_EVENT(MessageEvent, String, sender, String, message);
}  // namespace dw
