/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "io/File.h"

namespace dw {
Context::Context(const String& base_path, const String& pref_path)
    : config_(Json::object()), base_path_(base_path), pref_path_(pref_path) {
}

Context::~Context() {
}

Module* Context::module(Type module_type) const {
    auto it = modules_.find(module_type);
    if (it != modules_.end()) {
        return (*it).second.get();
    }
    return nullptr;
}

Module* Context::addModule(UniquePtr<Module> module) {
    Module* subsystem_ptr = module.get();
    module_init_order_.emplace_back(module->type());
    modules_.emplace(module->type(), std::move(module));
    return subsystem_ptr;
}

void Context::removeModule(Type module_type) {
    modules_.erase(module_type);
    module_init_order_.erase(
        std::find(module_init_order_.begin(), module_init_order_.end(), module_type));
}

void Context::clearModules() {
    for (auto it = module_init_order_.rbegin(); it != module_init_order_.rend(); ++it) {
        modules_.erase(*it);
    }
    module_init_order_.clear();
    assert(modules_.empty());
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
