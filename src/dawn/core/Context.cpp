/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "io/File.h"

namespace dw {
Context::Context(String basePath, String prefPath) : base_path_{basePath}, pref_path_{prefPath} {
}

Context::~Context() {
}

Object* Context::subsystem(StringHash subsystemType) {
    auto it = subsystems_.find(subsystemType);
    if (it != subsystems_.end()) {
        return (*it).second.get();
    }
    return nullptr;
}

void Context::addSubsystem(UniquePtr<Object> subsystem) {
    subsystems_.emplace(subsystem->type(), std::move(subsystem));
}

void Context::removeSubsystem(StringHash subsystemType) {
    subsystems_.erase(subsystemType);
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

void Context::loadConfig(const String& configFile) {
    File inFile(this, configFile, FileMode::Read);
    config_ = Json::parse(stream::read<String>(inFile));
}

void Context::saveConfig(const String& configFile) {
    File outFile(this, configFile, FileMode::Write);
    stream::write(outFile, config_.dump(4));
}

const String& Context::basePath() const {
    return base_path_;
}

const String& Context::prefPath() const {
    return pref_path_;
}
}
