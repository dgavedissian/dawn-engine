/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include <json.hpp>

#include "core/math/StringHash.h"
#include "core/Collections.h"

namespace dw {

class Module;

using Json = nlohmann::json;

/// The Context is a class which acts as a container of modules, which are low level engine
/// services such as the resource manager, renderer, filesystem, entity manager, etc. The context
/// also contains the engine and game configuration encoded as JSON.
class DW_API Context {
public:
    Context(const String& base_path, const String& pref_path);
    Context(Context& other) = delete;
    ~Context();

    /// Non-copyable.
    Context& operator=(const Context& other) = delete;

    /// Accesses a module by type hash. Requires a downcast.
    /// @param module_type Module type hash.
    /// @return A pointer to the module instance, or nullptr otherwise.
    Module* module(StringHash module_type) const;

    /// Adds a module to this context.
    /// @param module Module instance.
    Module* addModule(UniquePtr<Module> module);

    /// Removes a module contained within the context, calling the modules destructor.
    /// @param module_type Module type hash.
    void removeModule(StringHash module_type);

    /// Clears all modules from this context. Equivalent to calling removeModule on
    /// every module.
    void clearModules();

    /// Accesses a module by type.
    /// @tparam T Module type.
    /// @return A pointer to the module instance, or nullptr otherwise.
    template <typename T> T* module() const;

    /// Constructs a new module and adds it to the context.
    /// @tparam T Module type.
    /// @tparam Args Argument types.
    /// @param args Arguments for the module constructor.
    template <typename T, typename... Args> T* addModule(Args... args);

    /// Removes a module contained within this context, calling the modules destructor.
    /// @tparam T Module type.
    template <typename T> void removeModule();

    /// Access the config root.
    Json& config();

    /// Access the config root.
    const Json& config() const;

    /// Load the configuration.
    void loadConfig(const String& config_file);

    /// Save the configuration.
    void saveConfig(const String& config_file);

    /// Set the default configuration.
    void setDefaultConfig();

    /// Get the base path of the application.
    const String& basePath() const;

    /// Get the preferences path of the application.
    const String& prefPath() const;

private:
    HashMap<StringHash, UniquePtr<Module>> modules_;
    Vector<StringHash> module_init_order_;

    // Engine configuration.
    Json config_;

    // File paths.
    String base_path_;
    String pref_path_;
};

template <typename T> T* Context::module() const {
    return static_cast<T*>(module(T::typeStatic()));
}

template <typename T, typename... Args> T* Context::addModule(Args... args) {
    return static_cast<T*>(addModule(makeUnique<T>(this, std::forward<Args>(args)...)));
}

template <typename T> void Context::removeModule() {
    removeModule(T::typeStatic());
}
}  // namespace dw
