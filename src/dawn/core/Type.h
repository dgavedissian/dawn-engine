/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include <typeinfo>

namespace dw {
using Type = std::size_t;

class DW_API TypeInfo {
public:
    explicit TypeInfo(const std::type_info& t);
    ~TypeInfo();

    Type type() const;
    String typeName() const;

    bool operator==(const TypeInfo& other) const;
    bool operator!=(const TypeInfo& other) const;

private:
    const std::type_info& type_info_;
    String type_name_;
};
}  // namespace dw

namespace std {
template <> struct hash<dw::TypeInfo> {
    typedef dw::TypeInfo argument_type;
    typedef std::size_t result_type;
    result_type operator()(const argument_type& k) const {
        return static_cast<result_type>(k.type());
    }
};
}  // namespace std
