/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"

namespace dw {

TypeInfo::TypeInfo(const std::type_info& t) : mType(mTypeName), mTypeName(t.name()) {
}

TypeInfo::~TypeInfo() {
}

StringHash TypeInfo::getType() const {
    return mType;
}

String TypeInfo::getTypeName() const {
    return mTypeName;
}

Object::Object(Context* context) : mContext(context) {
}

Object::~Object() {
}

Context* Object::getContext() const {
    return mContext;
}

Logger& Object::getLog() const {
    return *getSubsystem<Logger>();
}
}
