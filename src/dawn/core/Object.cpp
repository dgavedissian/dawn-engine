/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"

namespace dw {

TypeInfo::TypeInfo(const std::type_info& t) {
    mTypeName = String(t.name());
    mType = StringHash(mTypeName);
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
