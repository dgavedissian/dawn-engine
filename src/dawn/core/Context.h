/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include <json.hpp>
#include "ontology/World.hpp"
#include "core/Collections.h"
#include "math/StringHash.h"

namespace dw {

class Object;
class ConfigNode;
class Entity;

using Json = nlohmann::json;

class DW_API Context {
public:
    Context(String basePath, String prefPath);
    Context(Context& other) = delete;
    ~Context();

    Context& operator=(const Context& other) = delete;

    // Subsystems
    Object* subsystem(StringHash subsystemType);
    void addSubsystem(UniquePtr<Object> subsystem);
    void removeSubsystem(StringHash subsystemType);
    void clearSubsystems();

    // Convenient template methods for subsystems
    template <typename T> T* subsystem() {
        return static_cast<T*>(subsystem(T::typeStatic()));
    }

    template <typename T, typename... Args> void addSubsystem(Args... args) {
        addSubsystem(makeUnique<T>(std::forward<Args>(args)...));
    }

    template <typename T> void removeSubsystem() {
        removeSubsystem(T::typeStatic());
    }

    /// Access the config root.
    Json& config();

    /// Access the config root.
    const Json& config() const;

    /// Load the configuration.
    void loadConfig(const String& configFile);

    /// Save the configuration.
    void saveConfig(const String& configFile);

    /// Get the base path of the application.
    /// @returns The base path
    const String& basePath() const;

    /// Get the preferences path of the application.
    /// @returns The pref path
    const String& prefPath() const;

private:
    HashMap<StringHash, UniquePtr<Object>> subsystems_;

    // Engine configuration.
    Json config_;

    // File paths.
    String base_path_;
    String pref_path_;

    // Ontology world.
    Ontology::World ontology_world_;
    friend class EntityManager;
    friend class SystemManager;
};
}
