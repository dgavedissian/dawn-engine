/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "core/Object.h"

namespace dw {
class DW_API Module : public Object {
public:
    DW_OBJECT(Module)

    Module(Context* ctx);
    virtual ~Module();

    // Checked subsystem access (to enforce dependencies).
    Module* moduleByType(const TypeInfo& subsystem_type) const override;

protected:
    template <typename... T> void setDependencies() {
        HashSet<TypeInfo> new_dependencies = {T::typeInfoStatic()...};
        verifyDependencies(new_dependencies);
        addDependencies(new_dependencies);
    }

    template <typename... T> void setOptionalDependencies() {
        addDependencies({T::typeInfoStatic()...});
    }

private:
    HashSet<Type> dependencies_;

private:
    void verifyDependencies(const HashSet<TypeInfo>& dependencies);
    void addDependencies(const HashSet<TypeInfo>& dependencies);
};
}  // namespace dw