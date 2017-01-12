/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
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

// Specialisation of std::hash used for HashMap<K, V>
namespace std {
template <> struct hash<dw::StringHash> {
    typedef dw::StringHash argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& s) const {
        return s.value();
    }
};
}
