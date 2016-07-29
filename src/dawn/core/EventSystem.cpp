/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "core/EventSystem.h"
#include "core/Timer.h"

namespace dw {

const EventType EvtData_Exit::eventType(0x98aceab8);
const EventType EvtData_SendMessage::eventType(0x244cec1b);
const EventType EvtData_Message::eventType(0x8d859562);

EventSystem::EventSystem()
    : mActiveQueue(0),
      mProcessingEvents(false)
{
}

EventSystem::~EventSystem()
{
}

bool EventSystem::addListener(const EventListenerDelegate& eventDelegate, const EventType& type)
{
    if (mProcessingEvents)
    {
        mAddedEventListeners[type].push_back(eventDelegate);
    }
    else
    {
        // This will find or create the entry
        auto& eventListenerList = mEventListeners[type];
        for (auto delegateFunc : eventListenerList)
        {
            if (eventDelegate == delegateFunc)
            {
                // WARNING: Attempting to double-register a delegate
                return false;
            }
        }

        eventListenerList.push_back(eventDelegate);
    }

    return true;
}

bool EventSystem::removeListener(const EventListenerDelegate& eventDelegate, const EventType& type)
{
    if (mProcessingEvents)
    {
        mRemovedEventListeners[type].push_back(eventDelegate);
        return true;
    }
    else
    {
        bool success = false;
        auto findIt = mEventListeners.find(type);
        if (findIt != mEventListeners.end())
        {
            auto& Listeners = findIt->second;
            for (auto it = Listeners.begin(); it != Listeners.end(); ++it)
            {
                if (eventDelegate == (*it))
                {
                    Listeners.erase(it);
                    success = true;
                    break;
                }
            }
        }

        return success;
    }
}

void EventSystem::removeAllListeners(const EventListenerDelegate& eventDelegate)
{
    for (auto listenerPair : mEventListeners)
    {
        removeListener(eventDelegate, listenerPair.first);
    }
}

bool EventSystem::triggerEvent(const EventDataPtr& eventData) const
{
    bool processed = false;

    auto findIt = mEventListeners.find(eventData->getType());
    if (findIt != mEventListeners.end())
    {
        auto& eventListenerList = findIt->second;
        for (auto delegateFunction : eventListenerList)
        {
            delegateFunction(eventData);    // call the delegate
            processed = true;
        }
    }

    return processed;
}

bool EventSystem::queueEvent(const EventDataPtr& eventData)
{
    assert(mActiveQueue >= 0);
    assert(mActiveQueue < EVENTSYSTEM_NUM_QUEUES);

    // Make sure the event is valid
    if (!eventData)
    {
        // ERROR: Invalid event in queueEvent()
        return false;
    }

    auto findIt = mEventListeners.find(eventData->getType());
    if (findIt != mEventListeners.end())
    {
        mQueues[mActiveQueue].push_back(eventData);
        return true;
    }
    else
    {
        return false;
    }
}

bool EventSystem::abortEvent(const EventType& type, bool allOfType /*= false*/)
{
    assert(mActiveQueue >= 0);
    assert(mActiveQueue < EVENTSYSTEM_NUM_QUEUES);

    bool success = false;
    auto findIt = mEventListeners.find(type);

    if (findIt != mEventListeners.end())
    {
        auto& eventQueue = mQueues[mActiveQueue];
        auto it = eventQueue.begin();

        while (it != eventQueue.end())
        {
            // Removing an item from the queue will invalidate the iterator, so have
            // it point to the next member. All work inside this loop will be done
            // using thisIt.
            auto currentIt = it;
            ++it;

            if ((*currentIt)->getType() == type)
            {
                eventQueue.erase(currentIt);
                success = true;

                if (!allOfType)
                    break;
            }
        }
    }

    return success;
}

bool EventSystem::update(double maxDuration)
{
    time::TimePoint now = time::beginTiming();

    // swap active queues and clear the new queue after the swap
    int queueToProcess = mActiveQueue;
    mActiveQueue = (mActiveQueue + 1) % EVENTSYSTEM_NUM_QUEUES;
    mQueues[mActiveQueue].clear();

    // Process the queue
    mProcessingEvents = true;
    while (!mQueues[queueToProcess].empty())
    {
        // Pop the front of the queue
        EventDataPtr eventData = mQueues[queueToProcess].front();
        mQueues[queueToProcess].pop_front();
        const EventType& eventType = eventData->getType();

        // Find all the delegate functions registered for this event
        auto findIt = mEventListeners.find(eventType);
        if (findIt != mEventListeners.end())
        {
            // Call each Listener
            auto& eventListeners = (*findIt).second;
            for (auto delegateFunction : eventListeners)
                delegateFunction(eventData);
        }

        // Check to see if time ran out
        if (time::elapsed(now) >= maxDuration)
            break;
    }

    mProcessingEvents = false;

    // If we couldn't process all of the events, push the remaining events to the new active queue
    // Note: To preserve sequencing, go back-to-front, inserting them at the head of the active
    // queue
    bool queueFlushed = mQueues[queueToProcess].empty();
    if (!queueFlushed)
    {
        while (!mQueues[queueToProcess].empty())
        {
            EventDataPtr pEvent = mQueues[queueToProcess].back();
            mQueues[queueToProcess].pop_back();
            mQueues[mActiveQueue].push_front(pEvent);
        }
    }

    // If any Listeners were queued for addition or removal whilst processing events, add them now
    for (auto Listener : mAddedEventListeners)
    {
        for (auto delegateFunc : Listener.second)
            addListener(delegateFunc, Listener.first);
    }
    mAddedEventListeners.clear();

    for (auto Listener : mRemovedEventListeners)
    {
        for (auto delegateFunc : Listener.second)
            removeListener(delegateFunc, Listener.first);
    }
    mRemovedEventListeners.clear();

    return queueFlushed;
}

}
