/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "core/StringUtils.h"
#if defined(DW_GCC) || defined(DW_CLANG)
#include <cxxabi.h>
#endif

namespace dw {
TypeInfo::TypeInfo(const std::type_info& t) : type_info_{t}, type_name_{} {
#if defined(DW_GCC) || defined(DW_CLANG)
    int status = 0;
    char* demangled_name = abi::__cxa_demangle(t.name(), nullptr, nullptr, &status);
    type_name_ = status == 0 ? demangled_name : t.name();
    free(demangled_name);
#elif defined(DW_MSVC)
    type_name_ = t.name();
    type_name_ = str::replace(type_name_, "class ", "");
    type_name_ = str::replace(type_name_, "struct ", "");
#else
    type_name_ = t.name();
#endif
}

TypeInfo::~TypeInfo() {
}

Type TypeInfo::type() const {
    return type_info_.hash_code();
}

String TypeInfo::typeName() const {
    return type_name_;
}

bool TypeInfo::operator==(const TypeInfo& other) const {
    return type_info_ == other.type_info_;
}

bool TypeInfo::operator!=(const TypeInfo& other) const {
    return type_info_ != other.type_info_;
}

Object::Object(Context* context) : context_{context} {
}

Object::~Object() {
}

Context* Object::context() const {
    return context_;
}

Logger& Object::log() const {
    return module<Logger>()->withObjectName(typeName());
}

Module* Object::moduleByType(const TypeInfo& subsystem_type) const {
    return context_->module(subsystem_type.type());
}
}  // namespace dw
