/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

namespace dw {

#define DW_OBJECT(type) \
    typedef type Type; \
    virtual TypeInfo getTypeInfo() const { return typeid(type); }

class Object
{
    Object();
    virtual ~Object();
    const String getTypeName() const { return String(getTypeInfo().name()); }
    virtual TypeInfo getTypeInfo() const = 0;
};

}
