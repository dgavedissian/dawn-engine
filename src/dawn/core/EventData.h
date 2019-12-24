/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "core/Delegate.h"
#include "core/math/StringHash.h"

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
        virtual bool equals(const EventForwarderBase& other) const = 0;
    };

    template <typename T, typename E> class EventForwarder : public EventForwarderBase {
    public:
        typedef void (T::*MemberEventHandler)(const E&);

        EventForwarder(T* object_ptr, MemberEventHandler member_func_ptr)
            : object_ptr(object_ptr), member_func_ptr(member_func_ptr) {
        }

        void onEvent(EventDataPtr base_event_data) {
            E* event_data = static_cast<E*>(base_event_data.get());
            (object_ptr->*member_func_ptr)(*event_data);
        }

        bool equals(const EventForwarderBase& other) const override {
            const auto& other_forwarder = static_cast<const EventForwarder<T, E>&>(other);
            return object_ptr == other_forwarder.object_ptr &&
                   member_func_ptr == other_forwarder.member_func_ptr;
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
        auto* const delegate_this = static_cast<Delegate<void(EventDataPtr)>*>(this);
        *delegate_this = makeDelegate(event_forwarder.get(), &EventForwarder<T, E>::onEvent);
    }

    bool equals(const EventDelegate& other) const {
        return event_forwarder_->equals(*other.event_forwarder_);
    }

private:
    SharedPtr<EventForwarderBase> event_forwarder_;
};
}  // namespace dw

// Event definition generator macro.
#define DEFINE_EMPTY_EVENT(event_name)         \
    struct event_name : public dw::EventData { \
        event_name() = default;                \
        static dw::EventType typeStatic() {    \
            return Hash(#event_name);          \
        }                                      \
        dw::EventType type() const override {  \
            return Hash(#event_name);          \
        }                                      \
        dw::String name() const override {     \
            return #event_name;                \
        }                                      \
    }
#define DEFINE_EVENT(event_name, ...)                                                    \
    struct event_name : public dw::EventData {                                           \
        event_name(EVENT_CTOR_ARGS(__VA_ARGS__)) : EVENT_INITIALISER_LIST(__VA_ARGS__) { \
        }                                                                                \
        static dw::EventType typeStatic() {                                              \
            return Hash(#event_name);                                                    \
        }                                                                                \
        dw::EventType type() const override {                                            \
            return Hash(#event_name);                                                    \
        }                                                                                \
        dw::String name() const override {                                               \
            return #event_name;                                                          \
        }                                                                                \
        EVENT_DATA_FIELDS(__VA_ARGS__)                                                   \
    }
#define EVENT_CTOR_ARGS(...) CALL_OVERLOAD(EVENT_CTOR_ARGS, __VA_ARGS__)
#define EVENT_CTOR_ARGS2(type1, name1) const type1& name1
#define EVENT_CTOR_ARGS4(type1, name1, type2, name2) const type1 &name1, const type2 &name2
#define EVENT_CTOR_ARGS6(type1, name1, type2, name2, type3, name3) \
    const type1 &name1, const type2 &name2, const type3 &name3
#define EVENT_INITIALISER_LIST(...) CALL_OVERLOAD(EVENT_INITIALISER_LIST, __VA_ARGS__)
#define EVENT_INITIALISER_LIST2(type1, name1) name1(name1)
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
