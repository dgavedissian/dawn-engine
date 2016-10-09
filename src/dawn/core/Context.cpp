/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "core/Context.h"

#include "engine/ConfigNode.h"
#include "io/File.h"

namespace dw {

Context::Context(String basePath, String prefPath) : mBasePath(basePath), mPrefPath(prefPath) {
    mConfig = makeUnique<ConfigNode>(this);
}

Context::~Context() {
}


void Context::addSubsystem(Object* subsystem) {
    mSubsystems[subsystem->getType()] = UniquePtr<Object>(subsystem);
}

Object* Context::getSubsystem(StringHash subsystemType) {
    return mSubsystems[subsystemType].get();
}

void Context::removeSubsystem(StringHash subsystemType) {
    mSubsystems.erase(subsystemType);
}

void Context::clearSubsystems() {
    mSubsystems.clear();
}

ConfigNode& Context::getConfig() {
    return *mConfig;
}

const ConfigNode& Context::getConfig() const {
    return *mConfig;
}

void Context::loadConfig(const String& configFile) {
    File inFile(this, configFile, FileMode::Read);
    mConfig->load(inFile);
}

void Context::saveConfig(const String& configFile) {
    File outFile(this, configFile, FileMode::Write);
    mConfig->save(outFile);
}
}
