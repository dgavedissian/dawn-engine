/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "math/StringHash.h"

namespace dw {

class Object;

class DW_API Context {
public:
    Context(String basePath, String prefPath);
    ~Context();

    // Subsystems
    void addSubsystem(Object* subsystem);
    Object* getSubsystem(StringHash subsystemType);
    void removeSubsystem(StringHash subsystemType);
    void clearSubsystems();

    // Convenient template methods for subsystems
    template <class T> T* getSubsystem() {
        return static_cast<T*>(getSubsystem(T::getTypeStatic()));
    }

    template <class T> void removeSubsystem() {
        removeSubsystem(T::getTypeStatic());
    }

    /// Get the base path of the application
    /// @returns The base path
    const String& getBasePath() const {
        return mBasePath;
    }

    /// Get the preferences path of the application
    /// @returns The pref path
    const String& getPrefPath() const {
        return mPrefPath;
    }

private:
    // HashMaps require an enum type (e.g. an integer), so we use the StringHash's internal type
    HashMap<uint32_t, SharedPtr<Object>> mSubsystems;

    // File paths
    String mBasePath;
    String mPrefPath;
};
}