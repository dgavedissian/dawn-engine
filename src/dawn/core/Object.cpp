/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"

namespace dw {

TypeInfo::TypeInfo(const std::type_info& t) : type_(type_name_), type_name_(t.name()) {
}

TypeInfo::~TypeInfo() {
}

StringHash TypeInfo::type() const {
    return type_;
}

String TypeInfo::typeName() const {
    return type_name_;
}

Object::Object(Context* context) : context_(context) {
}

Object::~Object() {
}

Context* Object::context() const {
    return context_;
}

Logger& Object::log() const {
    return *subsystem<Logger>();
}
}
