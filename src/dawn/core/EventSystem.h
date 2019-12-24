/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "core/EventData.h"

namespace dw {
#define EVENTSYSTEM_NUM_QUEUES 2

template <typename T, typename E>
EventDelegate makeEventDelegate(T* ptr, void (T::*handler)(const E&)) {
    return EventDelegate(ptr, handler);
}

template <typename E, typename T> using EventHandler = void (T::*)(const E&);

using EventHandlerId = uint;

class DW_API EventSystem : public Object {
private:
    struct EventHandlerBinding {
        void* listener;
        EventHandlerId id;
        EventDelegate event_delegate;

        EventHandlerBinding(void* listener, EventHandlerId id, EventDelegate event_delegate)
            : listener(listener), id(id), event_delegate(std::move(event_delegate)) {
        }
    };

    using EventHandlerBindingList = Vector<EventHandlerBinding>;

public:
    DW_OBJECT(EventSystem);

    EventSystem(Context* context);
    virtual ~EventSystem();

    /// Registers a delegate function that will get called when the event type is triggered.
    template <typename E, typename T>
    EventHandlerId addListener(T* listener, EventHandler<E, T> handler);

    /// Removes a delegate / event type pairing from the internal tables. Returns false if the
    /// pairing was not found.
    bool removeListener(EventHandlerId event_handler_id);

    /// Removes all delegate / event type pairings for a given listener.
    template <typename T> bool removeAllListeners(T* listener);

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
    void addDelegate(EventType type, EventHandlerBinding binding);

    // Event listeners.
    EventHandlerId next_event_handler_id_;
    HashMap<EventType, EventHandlerBindingList> event_listeners_;

    // Queues and index of actively processing queue; events enque to the opposing queue.
    Deque<EventDataPtr> queues_[EVENTSYSTEM_NUM_QUEUES];
    int active_queue_;

    // This flag is set when events are being processed. In this case, we queue changes to the event
    // listeners hash maps.
    bool processing_events_;
    Vector<Pair<EventType, EventHandlerBinding>> pending_added_event_listeners_;
    Vector<EventHandlerId> pending_remove_event_listeners_;
};

template <typename E, typename T>
EventHandlerId EventSystem::addListener(T* listener, EventHandler<E, T> handler) {
    EventDelegate event_delegate = makeEventDelegate<T, E>(listener, handler);
    auto handler_id = next_event_handler_id_++;
    addDelegate(E::typeStatic(),
                EventHandlerBinding(static_cast<void*>(listener), handler_id, event_delegate));
    return handler_id;
}

template <typename T> bool EventSystem::removeAllListeners(T* listener) {
    // Search all event types for this listener, and erase once found.
    bool result = false;
    for (auto& event_type_pair : event_listeners_) {
        auto& binding_list = event_type_pair.second;
        for (auto it = binding_list.begin(); it != binding_list.end();) {
            if (it->listener == listener) {
                if (processing_events_) {
                    pending_remove_event_listeners_.emplace_back(it->id);
                    ++it;
                } else {
                    it = binding_list.erase(it);
                }
                result = true;
            } else {
                ++it;
            }
        }
    }
    return result;
}

template <typename T, typename... Args> bool EventSystem::triggerEvent(Args&&... args) const {
    bool processed = false;

    auto event_data = makeShared<T>(std::forward<Args>(args)...);

    auto event_type_listeners = event_listeners_.find(event_data->type());
    if (event_type_listeners != event_listeners_.end()) {
        auto& listeners = event_type_listeners->second;
        for (const auto& binding : listeners) {
            binding.event_delegate(event_data);
            processed = true;
        }
    }

    return processed;
}

template <typename T, typename... Args> bool EventSystem::queueEvent(Args&&... args) const {
    assert(active_queue_ >= 0);
    assert(active_queue_ < EVENTSYSTEM_NUM_QUEUES);

    auto event_data = makeShared<T>(std::forward<Args>(args)...);

    auto event_type_listeners = event_listeners_.find(event_data->type());
    if (event_type_listeners != event_listeners_.end()) {
        queues_[active_queue_].push_back(event_data);
        return true;
    }
    return false;
}

DEFINE_EMPTY_EVENT(ExitEvent);
DEFINE_EVENT(SendMessageEvent, String, sender, String, message);
DEFINE_EVENT(MessageEvent, String, sender, String, message);
}  // namespace dw
