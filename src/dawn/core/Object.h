/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "core/Context.h"
#include "core/EventData.h"
#include "math/StringHash.h"

namespace dw {
class Logger;
class EventSystem;

using TypeId = StringHash;

class DW_API TypeInfo {
public:
    TypeInfo(const std::type_info& t);
    ~TypeInfo();

    StringHash type() const;
    String typeName() const;

private:
    String type_name_;
    StringHash type_name_hash_;
};

#define DW_OBJECT(T)                                 \
    using Type = T;                                  \
    virtual dw::StringHash type() const override {   \
        return typeInfo().type();                    \
    }                                                \
    virtual dw::String typeName() const override {   \
        return typeInfo().typeName();                \
    }                                                \
    virtual dw::TypeInfo typeInfo() const override { \
        return typeInfoStatic();                     \
    }                                                \
    static dw::StringHash typeStatic() {             \
        return typeInfoStatic().type();              \
    }                                                \
    static dw::String typeNameStatic() {             \
        return typeInfoStatic().typeName();          \
    }                                                \
    static const dw::TypeInfo& typeInfoStatic() {    \
        static dw::TypeInfo ti(typeid(Type));        \
        return ti;                                   \
    }

class DW_API Object {
public:
    Object(Context* context);
    virtual ~Object();

    /// Returns the context that this object is associated with.
    Context* context() const;

    /// A convenient wrapper for context()->subsystem<Logger>()->withSection(typeName())
    Logger& log() const;

    /// A convenient wrapper for
    /// context()->subsystem<EventSystem>()->triggerEvent(makeShared<T>(...))
    template <typename T, typename... Args> bool triggerEvent(Args&&... args) const;

    /// A convenient wrapper for context()->subsystem<EventSystem>()->addListener(delegate,
    /// E::eventType)
    template <typename E> bool addEventListener(const EventDelegate& delegate);

    /// A convenient wrapper for context()->subsystem<EventSystem>()->removeListener(delegate,
    /// E::eventType)
    template <typename E> bool removeEventListener(const EventDelegate& delegate);

    /// A convenient wrapper for context().subsystem<T>().
    /// @tparam T Subsystem type.
    /// @return Subsystem instance.
    template <typename T> T* subsystem() const;

    virtual StringHash type() const = 0;
    virtual String typeName() const = 0;
    virtual TypeInfo typeInfo() const = 0;

protected:
    Context* context_;
};
}  // namespace dw

#include "core/EventSystem.h"

namespace dw {
template <typename T, typename... Args> bool Object::triggerEvent(Args&&... args) const {
    return context_->subsystem<EventSystem>()->triggerEvent(
        makeShared<T>(std::forward<Args>(args)...));
};

template <typename E> bool Object::addEventListener(const EventDelegate& delegate) {
    return context_->subsystem<EventSystem>()->addListener(delegate, E::eventType);
}

template <typename E> bool Object::removeEventListener(const EventDelegate& delegate) {
    return context_->subsystem<EventSystem>()->removeListener(delegate, E::eventType);
}

template <typename T> T* Object::subsystem() const {
    return context_->subsystem<T>();
}
}  // namespace dw
