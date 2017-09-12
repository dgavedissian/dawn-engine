/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "io/File.h"

namespace dw {
Context::Context(String base_path, String pref_path)
    : config_(Json::object()), base_path_(base_path), pref_path_(pref_path) {
}

Context::~Context() {
}

Object* Context::subsystem(StringHash subsystem_type) const {
    auto it = subsystems_.find(subsystem_type);
    if (it != subsystems_.end()) {
        return (*it).second.get();
    }
    return nullptr;
}

Object* Context::addSubsystem(UniquePtr<Object> subsystem) {
    Object* subsystem_ptr = subsystem.get();
    subsystems_.emplace(subsystem->type(), std::move(subsystem));
    return subsystem_ptr;
}

void Context::removeSubsystem(StringHash subsystem_type) {
    subsystems_.erase(subsystem_type);
}

void Context::clearSubsystems() {
    subsystems_.clear();
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
    config_["window_width"] = 1280;
    config_["window_height"] = 800;
}

const String& Context::basePath() const {
    return base_path_;
}

const String& Context::prefPath() const {
    return pref_path_;
}
}  // namespace dw
