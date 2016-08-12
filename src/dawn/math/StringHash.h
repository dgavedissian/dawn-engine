/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

namespace dw {

class DW_API StringHash {
public:
    using HashType = u32;

    StringHash();
    explicit StringHash(HashType value);
    StringHash(const char* str);
    StringHash(const String& str);

    StringHash operator+(const StringHash& rhs) const;
    StringHash& operator+=(const StringHash& rhs);
    bool operator==(const StringHash& rhs) const;
    bool operator!=(const StringHash& rhs) const;
    bool operator<(const StringHash& rhs) const;
    bool operator>(const StringHash& rhs) const;
    operator bool() const;

    HashType value() const;
    String toString() const;

    static HashType calculate(const char* str);

    static const StringHash ZERO;

private:
    HashType mValue;
};
}
