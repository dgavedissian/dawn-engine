/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "core/Module.h"

namespace dw {
Module::Module(Context* ctx) : Object(ctx) {
}

Module::~Module() {
}

Module* Module::moduleByType(const TypeInfo& subsystem_type) const {
    Module* subsystem = Object::moduleByType(subsystem_type);
    if (subsystem_type.type() != Logger::typeStatic() &&
        dependencies_.count(subsystem_type.type()) == 0) {
        // We should warn here, as we're trying to access an unspecified dependency.
        log().warn("Attempting to access subsystem {}, which is not declared as a dependency.",
                   subsystem_type.typeName());
    }
    return subsystem;
}

void Module::verifyDependencies(const HashSet<TypeInfo>& dependencies) {
    for (auto& dependency : dependencies) {
        Module* subsystem = Object::moduleByType(dependency);
        if (subsystem == nullptr) {
            log().error(
                "{} is declared as a subsystem dependency, but is not initialised. A crash will "
                "likely follow.",
                dependency.typeName());
        }
    }
}

void Module::addDependencies(const HashSet<TypeInfo>& dependencies) {
    for (auto& dependency : dependencies) {
        dependencies_.emplace(dependency.type());
    }
}
}  // namespace dw