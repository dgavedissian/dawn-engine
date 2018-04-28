/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "core/Delegate.h"

namespace dw {
using EventType = StringHash;

class DW_API EventData {
public:
    virtual ~EventData() = default;
    virtual EventType type() const = 0;
    virtual String name() const = 0;
};

using EventDataPtr = SharedPtr<EventData>;

// Subclass of Delegate, which contains a special constructor that will register an internal
// forwarding delegate which will automatically cast the event data type.
class EventDelegate : public Delegate<void(EventDataPtr)> {
public:
    class EventForwarderBase {
    public:
        virtual ~EventForwarderBase() = default;
    };

    template <typename T, typename E> class EventForwarder : public EventForwarderBase {
    public:
        typedef void (T::*MemberEventHandler)(const E&);

        EventForwarder(T* object_ptr, MemberEventHandler member_func_ptr)
            : object_ptr(object_ptr), member_func_ptr(member_func_ptr) {
        }

        void onEvent(EventDataPtr base_event_data) {
            E* event_data = (E*)base_event_data.get();
            (object_ptr->*member_func_ptr)(*event_data);
        }

    private:
        T* object_ptr;
        MemberEventHandler member_func_ptr;
    };

    template <typename T, typename E> EventDelegate(T* object, void (T::*func_ptr)(const E&)) {
        auto event_forwarder = makeShared<EventForwarder<T, E>>(object, func_ptr);
        event_forwarder_ = event_forwarder;

        // Use Delegate<...>'s assignment operator to assign the actual delegate which this is a
        // subclass of.
        *static_cast<Delegate<void(EventDataPtr)>*>(this) =
            makeDelegate(event_forwarder.get(), &EventForwarder<T, E>::onEvent);
    }

private:
    SharedPtr<EventForwarderBase> event_forwarder_;
};
}  // namespace dw