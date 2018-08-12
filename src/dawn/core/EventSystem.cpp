/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "core/EventSystem.h"
#include "core/Timer.h"

namespace dw {
EventSystem::EventSystem(Context* context)
    : Object(context), active_queue_(0), processing_events_(false) {
}

EventSystem::~EventSystem() {
}

bool EventSystem::abortEvent(const EventType& type, bool all_of_type /*= false*/) {
    assert(active_queue_ >= 0);
    assert(active_queue_ < EVENTSYSTEM_NUM_QUEUES);

    bool success = false;
    auto event_listener_it = event_listeners_.find(type);

    if (event_listener_it != event_listeners_.end()) {
        auto& event_queue = queues_[active_queue_];
        auto it = event_queue.begin();

        while (it != event_queue.end()) {
            // Removing an item from the queue will invalidate the iterator, so have it point to the
            // next member. All work inside this loop will be done using thisIt.
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
    }

    return success;
}

bool EventSystem::update(double max_duration) {
    time::TimePoint now = time::beginTiming();

    // swap active queues and clear the new queue after the swap
    int queue_to_process = active_queue_;
    active_queue_ = (active_queue_ + 1) % EVENTSYSTEM_NUM_QUEUES;
    queues_[active_queue_].clear();

    // Process the queue
    processing_events_ = true;
    while (!queues_[queue_to_process].empty()) {
        // Pop the front of the queue
        EventDataPtr event_data = queues_[queue_to_process].front();
        queues_[queue_to_process].pop_front();
        const EventType& event_type = event_data->type();

        // Find all the delegate functions registered for this event
        auto find_it = event_listeners_.find(event_type);
        if (find_it != event_listeners_.end()) {
            // Call each Listener
            auto& event_listeners = (*find_it).second;
            for (const auto& delegate_function : event_listeners)
                delegate_function(event_data);
        }

        // Check to see if time ran out
        if (time::elapsed(now) >= max_duration)
            break;
    }

    processing_events_ = false;

    // If we couldn't process all of the events, push the remaining events to the new active queue
    // Note: To preserve sequencing, go back-to-front, inserting them at the head of the active
    // queue
    bool queue_flushed = queues_[queue_to_process].empty();
    if (!queue_flushed) {
        while (!queues_[queue_to_process].empty()) {
            EventDataPtr pEvent = queues_[queue_to_process].back();
            queues_[queue_to_process].pop_back();
            queues_[active_queue_].push_front(pEvent);
        }
    }

    // If any Listeners were queued for addition or removal whilst processing events, add them now
    for (auto listener : added_event_listeners_) {
        for (const auto& delegate_function : listener.second) {
            addDelegate(delegate_function, listener.first);
        }
    }
    added_event_listeners_.clear();

    for (auto listener : remove_event_listeners_) {
        for (const auto& delegate_function : listener.second) {
            removeDelegate(delegate_function, listener.first);
        }
    }
    remove_event_listeners_.clear();

    return queue_flushed;
}

bool EventSystem::addDelegate(EventDelegate event_delegate, EventType type) {
    if (processing_events_) {
        added_event_listeners_[type].push_back(event_delegate);
    } else {
        // This will find or create the entry
        auto& event_listener_list = event_listeners_[type];
        for (const auto& delegate_function : event_listener_list) {
            if (event_delegate == delegate_function) {
                // WARNING: Attempting to double-register a delegate
                return false;
            }
        }

        event_listener_list.push_back(event_delegate);
    }

    return true;
}

bool EventSystem::removeDelegate(EventDelegate event_delegate, EventType type) {
    if (processing_events_) {
        remove_event_listeners_[type].push_back(event_delegate);
        return true;
    }
    bool success = false;
    auto find_it = event_listeners_.find(type);
    if (find_it != event_listeners_.end()) {
        auto& listeners = find_it->second;
        for (auto it = listeners.begin(); it != listeners.end(); ++it) {
            if (event_delegate == (*it)) {
                listeners.erase(it);
                success = true;
                break;
            }
        }
    }

    return success;
}
}  // namespace dw
