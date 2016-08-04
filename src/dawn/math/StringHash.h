/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

namespace dw {

class DW_API StringHash {
public:
    StringHash();
    explicit StringHash(uint32_t value);
    StringHash(const char* str);
    StringHash(const String& str);

    StringHash operator+(const StringHash& rhs) const;
    StringHash& operator+=(const StringHash& rhs);
    bool operator==(const StringHash& rhs) const;
    bool operator!=(const StringHash& rhs) const;
    bool operator<(const StringHash& rhs) const;
    bool operator>(const StringHash& rhs) const;
    operator bool() const;

    uint32_t value() const;
    String toString() const;

    static uint32_t calculate(const char* str);

    static const StringHash ZERO;

private:
    uint32_t mValue;
};
}
