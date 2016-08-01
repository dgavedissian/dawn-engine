/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

namespace dw {

class Context;

class TypeInfo {
public:
    TypeInfo(std::type_info t);
    ~TypeInfo();

    StringHash getType() const;
    String getTypeName() const;

private:
    StringHash mType;
    String mTypeName;
}
#define DW_OBJECT(type)                                \
    typedef type Type;                                 \
    virtual StringHash getType() const {               \
        return getTypeInfo().getType();                \
    }                                                  \
    virtual String getTypeName() const {               \
        return getTypeInfo().getTypeName();            \
    }                                                  \
    virtual TypeInfo getTypeInfo() const {             \
        return getTypeInfoStatic();                    \
    }                                                  \
    static StringHash getTypeStatic() {                \
        return getTypeInfoStatic().getType();          \
    }                                                  \
    static String getTypeNameStatic() {                \
        return getTypeInfoStatic().getTypeName();      \
    }                                                  \
    static const TypeInfo& getTypeInfoStatic() const { \
        static TypeInfo ti(typeid(type));              \
        return ti;                                     \
    }

class Object {

public:
    Object(Context* context);
    virtual ~Object();

    Context* getContext() const;

    virtual StringHash getType() const = 0;
    virtual String getTypeName() const = 0;
    virtual TypeInfo getTypeInfo() const = 0;

private:
    Context* mContext;
};
}
