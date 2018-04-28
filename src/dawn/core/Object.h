/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "core/Context.h"
#include "core/EventData.h"
#include "core/math/StringHash.h"

namespace dw {
using Type = std::size_t;

class DW_API TypeInfo {
public:
    TypeInfo(const std::type_info& t);
    ~TypeInfo();

    Type type() const;
    String typeName() const;

    bool operator==(const TypeInfo& other) const;
    bool operator!=(const TypeInfo& other) const;

private:
    const std::type_info& type_info_;
    String type_name_;
};
}  // namespace dw

namespace std {
template <> struct hash<dw::TypeInfo> {
    typedef dw::TypeInfo argument_type;
    typedef std::size_t result_type;
    result_type operator()(const argument_type& k) const {
        return static_cast<result_type>(k.type());
    }
};
}  // namespace std

#define DW_OBJECT(T)                                 \
    virtual dw::Type type() const override {         \
        return typeInfo().type();                    \
    }                                                \
    virtual dw::String typeName() const override {   \
        return typeInfo().typeName();                \
    }                                                \
    virtual dw::TypeInfo typeInfo() const override { \
        return typeInfoStatic();                     \
    }                                                \
    static dw::Type typeStatic() {                   \
        return typeInfoStatic().type();              \
    }                                                \
    static dw::String typeNameStatic() {             \
        return typeInfoStatic().typeName();          \
    }                                                \
    static const dw::TypeInfo& typeInfoStatic() {    \
        static dw::TypeInfo ti(typeid(T));           \
        return ti;                                   \
    }

namespace dw {
class Logger;
class EventSystem;

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

    /// A convenient wrapper for context()->subsystem(type);
    /// @param Subsystem type. Either T::typeInfoStatic() or dw::Object::typeInfo()
    virtual Module* moduleByType(const TypeInfo& subsystem_type) const;

    /// A type safe wrapper for moduleByType(T::typeInfoStatic()).
    /// @tparam T Subsystem type.
    /// @return Subsystem instance.
    template <typename T> T* module() const;

    virtual Type type() const = 0;
    virtual String typeName() const = 0;
    virtual TypeInfo typeInfo() const = 0;

protected:
    Context* context_;
};
}  // namespace dw

#include "core/Module.h"
#include "core/EventSystem.h"

namespace dw {
template <typename T, typename... Args> bool Object::triggerEvent(Args&&... args) const {
    return context_->module<EventSystem>()->triggerEvent(
        makeShared<T>(std::forward<Args>(args)...));
}

template <typename E> bool Object::addEventListener(const EventDelegate& delegate) {
    if (context_->module<EventSystem>()) {
        return context_->module<EventSystem>()->addListener(delegate, E::typeStatic());
    }
    return false;
}

template <typename E> bool Object::removeEventListener(const EventDelegate& delegate) {
    if (context_->module<EventSystem>()) {
        return context_->module<EventSystem>()->removeListener(delegate, E::typeStatic());
    }
    return false;
}

template <typename T> T* Object::module() const {
    return static_cast<T*>(moduleByType(T::typeInfoStatic()));
}
}  // namespace dw
