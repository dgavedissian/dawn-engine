/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

namespace dw {

class Context {
public:
    Context(String basePath, String prefPath);
    virtual ~Context();

    void addSubsystem(SharedPtr<Object> subsystem);

    template <class T> T* getSubsystem() {
        return static_cast<T*>(mSubsystems[T::getType()]);
    }

    /// Get the base path of the application
    /// @returns The base path
    const String& getBasePath() const {
        return mBasePath;
    }

    /// Get the preferences path of the application
    /// @returns The pref path
    const String& getPrefPath() const {
        return mPrefPath;
    }

private:
    HashMap<StringHash, SharedPtr<Object>> mSubsystems;

    // File paths
    String mBasePath;
    String mPrefPath;
};
}