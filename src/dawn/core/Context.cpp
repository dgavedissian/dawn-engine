/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "io/File.h"

namespace dw {
Context::Context(const String& base_path, const String& pref_path)
    : config_(Json::object()), base_path_(base_path), pref_path_(pref_path) {
}

Context::~Context() {
}

Subsystem* Context::subsystem(StringHash subsystem_type) const {
    auto it = subsystems_.find(subsystem_type);
    if (it != subsystems_.end()) {
        return (*it).second.get();
    }
    return nullptr;
}

Subsystem* Context::addSubsystem(UniquePtr<Subsystem> subsystem) {
    Subsystem* subsystem_ptr = subsystem.get();
    subsystem_init_order_.emplace_back(subsystem->type());
    subsystems_.emplace(subsystem->type(), std::move(subsystem));
    return subsystem_ptr;
}

void Context::removeSubsystem(StringHash subsystem_type) {
    subsystems_.erase(subsystem_type);
    subsystem_init_order_.erase(
        std::find(subsystem_init_order_.begin(), subsystem_init_order_.end(), subsystem_type));
}

void Context::clearSubsystems() {
    for (auto it = subsystem_init_order_.rbegin(); it != subsystem_init_order_.rend(); ++it) {
        subsystems_.erase(*it);
    }
    subsystem_init_order_.clear();
    assert(subsystems_.empty());
}

Json& Context::config() {
    return config_;
}

const Json& Context::config() const {
    return config_;
}

void Context::loadConfig(const String& config_file) {
    File inFile(this, config_file, FileMode::Read);
    config_ = Json::parse(stream::read<String>(inFile));
}

void Context::saveConfig(const String& config_file) {
    File outFile(this, config_file, FileMode::Write);
    stream::write(outFile, config_.dump(4));
}

void Context::setDefaultConfig() {
    config_["window_width"] = 1024;
    config_["window_height"] = 600;
}

const String& Context::basePath() const {
    return base_path_;
}

const String& Context::prefPath() const {
    return pref_path_;
}
}  // namespace dw
