/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "core/Context.h"
#include "core/EventData.h"
#include "core/Type.h"

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

    /// A convenient wrapper for module<Logger>()->withSection(typeName())
    Logger& log() const;

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
template <typename T> T* Object::module() const {
    return static_cast<T*>(moduleByType(T::typeInfoStatic()));
}
}  // namespace dw
