/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "core/EventSystem.h"
#include "core/Timer.h"

namespace dw {
EventSystem::EventSystem(Context* context)
    : Object(context), next_event_handler_id_(1), active_queue_(0), processing_events_(false) {
}

EventSystem::~EventSystem() {
}

bool EventSystem::removeListener(EventHandlerId event_handler_id) {
    if (processing_events_) {
        pending_remove_event_listeners_.emplace_back(event_handler_id);
        return true;
    }

    // Search all event types for this handler ID, and erase once found.
    for (auto& event_type_pair : event_listeners_) {
        auto& binding_list = event_type_pair.second;
        for (auto it = binding_list.begin(); it != binding_list.end(); ++it) {
            if (it->id == event_handler_id) {
                binding_list.erase(it);
                return true;
            }
        }
    }
    return false;
}

bool EventSystem::abortEvent(const EventType& type, bool all_of_type /*= false*/) {
    assert(active_queue_ >= 0);
    assert(active_queue_ < EVENTSYSTEM_NUM_QUEUES);

    const auto event_type_listeners = event_listeners_.find(type);

    if (event_type_listeners == event_listeners_.end()) {
        return false;
    }

    bool success = false;
    auto& event_queue = queues_[active_queue_];
    auto it = event_queue.begin();

    while (it != event_queue.end()) {
        // Removing an item from the queue will invalidate the iterator, so have it point to the
        // next member. All work inside this loop will be done using this iterator.
        auto current_it = it;
        ++it;

        if ((*current_it)->type() == type) {
            event_queue.erase(current_it);
            success = true;

            if (!all_of_type) {
                break;
            }
        }
    }

    return success;
}

bool EventSystem::update(double max_duration) {
    time::TimePoint now = time::beginTiming();

    // swap active queues and clear the new queue after the swap.
    const int queue_to_process = active_queue_;
    active_queue_ = (active_queue_ + 1) % EVENTSYSTEM_NUM_QUEUES;
    auto& current_queue = queues_[queue_to_process];
    auto& next_queue = queues_[active_queue_];
    next_queue.clear();

    // Process the queue, and record how many events processed.
    processing_events_ = true;
    while (!current_queue.empty()) {
        const auto event_data = current_queue.front();
        current_queue.pop_front();

        // Find all the delegate functions registered for this event.
        auto find_it = event_listeners_.find(event_data->type());
        if (find_it != event_listeners_.end()) {
            // Call each Listener.
            auto& event_listeners = (*find_it).second;
            for (const auto& binding : event_listeners) {
                binding.event_delegate(event_data);
            }
        }

        // Check to see if time ran out
        if (time::elapsed(now) >= max_duration) {
            break;
        }
    }
    processing_events_ = false;

    // If we couldn't process all of the events, push the remaining events to the new active queue
    // Note: To preserve sequencing, go back-to-front, inserting them at the head of the active
    // queue
    bool queue_flushed = current_queue.empty();
    if (!queue_flushed) {
        while (!current_queue.empty()) {
            EventDataPtr event_data = current_queue.back();
            current_queue.pop_back();
            next_queue.push_front(event_data);
        }
    }

    // If any changes to the lists were queued, process them now.
    for (const auto pending_listener : pending_added_event_listeners_) {
        addDelegate(pending_listener.first, pending_listener.second);
    }
    pending_added_event_listeners_.clear();
    for (auto id : pending_remove_event_listeners_) {
        removeListener(id);
    }
    pending_remove_event_listeners_.clear();
    return queue_flushed;
}

void EventSystem::addDelegate(EventType type, EventHandlerBinding binding) {
    if (processing_events_) {
        pending_added_event_listeners_.emplace_back(type, binding);
    } else {
        auto& event_listener_list = event_listeners_[type];
        event_listener_list.emplace_back(binding);
    }
}
}  // namespace dw
