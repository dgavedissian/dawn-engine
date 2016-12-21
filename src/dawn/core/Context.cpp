/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "io/File.h"

namespace dw {

Context::Context(String basePath, String prefPath) : mBasePath(basePath), mPrefPath(prefPath) {
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

json& Context::getConfig() {
    return mConfig;
}

const json& Context::getConfig() const {
    return mConfig;
}

void Context::loadConfig(const String& configFile) {
    File inFile(this, configFile, FileMode::Read);
    mConfig = json::parse(stream::read<String>(inFile));
}

void Context::saveConfig(const String& configFile) {
    File outFile(this, configFile, FileMode::Write);
    stream::write(outFile, mConfig.dump(4));
}
}
