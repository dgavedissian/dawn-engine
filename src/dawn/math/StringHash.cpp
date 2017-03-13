/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"

namespace dw {
const StringHash StringHash::ZERO;

StringHash::StringHash() : value_{0} {
}

StringHash::StringHash(HashType value) : value_{value} {
}

StringHash::StringHash(const char* str) : value_{calculate(str)} {
}

StringHash::StringHash(const String& str) : value_{calculate(str.c_str())} {
}

StringHash StringHash::operator+(const StringHash& rhs) const {
    StringHash ret;
    ret.value_ = value_ + rhs.value_;
    return ret;
}

StringHash& StringHash::operator+=(const StringHash& rhs) {
    value_ += rhs.value_;
    return *this;
}

bool StringHash::operator==(const StringHash& rhs) const {
    return value_ == rhs.value_;
}

bool StringHash::operator!=(const StringHash& rhs) const {
    return value_ != rhs.value_;
}

bool StringHash::operator<(const StringHash& rhs) const {
    return value_ < rhs.value_;
}

bool StringHash::operator>(const StringHash& rhs) const {
    return value_ > rhs.value_;
}

StringHash::operator bool() const {
    return value_ != 0;
}

uint32_t StringHash::value() const {
    return value_;
}

String StringHash::toString() const {
    return std::to_string(value_);
}

uint32_t StringHash::calculate(const char* str) {
    uint32_t hash = 0;

    if (!str) {
        return hash;
    }

    // Perform a case-insensitive hash using the SDBM algorithm
    while (*str) {
        hash = static_cast<uint32_t>(tolower(*str++)) + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}
}
