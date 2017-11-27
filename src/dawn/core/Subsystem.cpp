/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "core/Subsystem.h"

namespace dw {
Subsystem::Subsystem(Context* ctx) : Object(ctx) {
}

Subsystem::~Subsystem() {
}

Subsystem* Subsystem::subsystemByType(const TypeInfo& subsystem_type) const {
    Subsystem* subsystem = Object::subsystemByType(subsystem_type);
    if (subsystem_type.type() != Logger::typeStatic() &&
        dependencies_.count(subsystem_type.type()) == 0) {
        // We should warn here, as we're trying to access an unspecified dependency.
        log().warn("Attempting to access subsystem %s, which is not declared as a dependency.",
                   subsystem_type.typeName());
    }
    return subsystem;
}

void Subsystem::verifyDependencies(const HashSet<TypeInfo>& dependencies) {
    for (auto& dependency : dependencies) {
        Subsystem* subsystem = Object::subsystemByType(dependency);
        if (subsystem == nullptr) {
            log().error(
                "%s is declared as a subsystem dependency, but is not initialised. A crash will "
                "likely follow.",
                dependency.typeName());
        }
    }
}

void Subsystem::addDependencies(const HashSet<TypeInfo>& dependencies) {
    for (auto& dependency : dependencies) {
        dependencies_.emplace(dependency.type());
    }
}
}  // namespace dw