/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "core/Context.h"

namespace dw {

Context::Context() {
}

Context::~Context() {
}

void Context::addSubsystem(SharedPtr <Object> subsystem) {
    mSubsystems[subsystem->getType()] = subsystem;
}

}
