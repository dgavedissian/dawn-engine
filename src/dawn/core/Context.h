/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include <json.hpp>
using json = nlohmann::json;

#include "math/StringHash.h"

namespace dw {

class Object;
class ConfigNode;

class DW_API Context {
public:
    Context(String basePath, String prefPath);
    Context(Context& other) = delete;
    ~Context();

    Context& operator=(const Context& other) = delete;

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

    /// Access the config root
    json& getConfig();

    /// Access the config root
    const json& getConfig() const;

    /// Load the configuration
    void loadConfig(const String& configFile);

    /// Save the configuration
    void saveConfig(const String& configFile);

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
    HashMap<StringHash, UniquePtr<Object>> mSubsystems;

    // Engine configuration
    json mConfig;

    // File paths
    String mBasePath;
    String mPrefPath;
};
}
