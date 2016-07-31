/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

namespace dw {

class Context {
public:
    Context();
    virtual ~Context();

    void addSubsystem(SharedPtr <Object> subsystem);

    template<class T>
    T *getSubsystem() {
        return mSubsystems[T::getType()];
    }

private:
    HashMap <StringHash, SharedPtr<Object>> mSubsystems;
};

}