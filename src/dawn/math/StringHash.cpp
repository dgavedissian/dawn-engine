/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

NAMESPACE_BEGIN

const StringHash StringHash::ZERO;

StringHash::StringHash() :
    mValue(0)
{
}

StringHash::StringHash(uint32_t value) :
    mValue(value)
{
}

StringHash::StringHash(const char* str) :
    mValue(calculate(str))
{
}

StringHash::StringHash(const String& str) :
    mValue(calculate(str.c_str()))
{
}

StringHash StringHash::operator+(const StringHash& rhs) const
{
    StringHash ret;
    ret.mValue = mValue + rhs.mValue;
    return ret;
}

StringHash& StringHash::operator+=(const StringHash& rhs)
{
    mValue += rhs.mValue;
    return *this;
}

bool StringHash::operator==(const StringHash& rhs) const
{
    return mValue == rhs.mValue;
}

bool StringHash::operator!=(const StringHash& rhs) const
{
    return mValue != rhs.mValue;
}

bool StringHash::operator<(const StringHash& rhs) const
{
    return mValue < rhs.mValue;
}

bool StringHash::operator>(const StringHash& rhs) const
{
    return mValue > rhs.mValue;
}

StringHash::operator bool() const
{
    return mValue != 0;
}

uint32_t StringHash::value() const
{
    return mValue;
}

String StringHash::toString() const
{
    return std::to_string(mValue);
}

uint32_t StringHash::calculate(const char* str)
{
    uint32_t hash = 0;

    if (!str)
        return hash;

    // Perform a case-insensitive hash using the SDBM algorithm
    while (*str)
        hash = (uint32_t)tolower(*str++) + (hash << 6) + (hash << 16) - hash;
    return hash;
}

NAMESPACE_END

