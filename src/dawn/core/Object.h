/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "core/Context.h"
#include "math/StringHash.h"

namespace dw {

class DW_API TypeInfo {
public:
    TypeInfo(const std::type_info& t);
    ~TypeInfo();

    StringHash getType() const;
    String getTypeName() const;

private:
    StringHash mType;
    String mTypeName;
};

#define DW_OBJECT(T)                                    \
    typedef T Type;                                     \
    virtual dw::StringHash getType() const override {   \
        return getTypeInfo().getType();                 \
    }                                                   \
    virtual dw::String getTypeName() const override {   \
        return getTypeInfo().getTypeName();             \
    }                                                   \
    virtual dw::TypeInfo getTypeInfo() const override { \
        return getTypeInfoStatic();                     \
    }                                                   \
    static dw::StringHash getTypeStatic() {             \
        return getTypeInfoStatic().getType();           \
    }                                                   \
    static dw::String getTypeNameStatic() {             \
        return getTypeInfoStatic().getTypeName();       \
    }                                                   \
    static const dw::TypeInfo& getTypeInfoStatic() {    \
        static dw::TypeInfo ti(typeid(Type));           \
        return ti;                                      \
    }

class DW_API Object {
public:
    Object(Context* context);
    virtual ~Object();

    Context* getContext() const;

    // Convenient access to context methods
    template <class T> T* getSubsystem() {
        return mContext->getSubsystem<T>();
    }

    virtual StringHash getType() const = 0;
    virtual String getTypeName() const = 0;
    virtual TypeInfo getTypeInfo() const = 0;

protected:
    Context* mContext;
};
}
