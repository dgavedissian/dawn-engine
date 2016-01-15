/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

NAMESPACE_BEGIN

// This Singleton design is based off:
// https://github.com/nandor/MAZE/blob/master/maze/MZSingleton.h
template <class T>
class Singleton
{
public:
    // The holder is used to deallocate the instance - in case the derived class
    // does not implement a virtual destructor
    class Holder
    {
    public:
        Holder() : mInstance(nullptr)
        {
        }

        ~Holder()
        {
            if (mInstance != nullptr)
            {
                assert(false && "A singleton hasn't been released properly!");
            }
        }

        void setInstance(T* inst)
        {
            mInstance = inst;
        }

        void release()
        {
            if (mInstance != nullptr)
            {
                delete mInstance;
            }

            mInstance = nullptr;
        }

    private:
        T* mInstance;
    };

    // Singletons are initialized by calling the constructor of the class at
    // startup, so no thread safety is needed
    Singleton()
    {
        assert(!mInstance);
        mInstance = static_cast<T*>(this);
        mHolder.setInstance(mInstance);
    }

    // Deallocate the instance
    static void release()
    {
        mHolder.release();
        mInstance = nullptr;
    }

    // Returns a reference to the singleton instance
    static T& inst()
    {
        return *mInstance;
    }

    // Returns a pointer to the singleton instance
    static T* ptr()
    {
        return mInstance;
    }

private:
    // Do not allow copying
    Singleton(const Singleton<T>&);
    Singleton& operator=(const Singleton<T>&);

    static Holder mHolder;
    static T* mInstance;
};

template <class T>
typename Singleton<T>::Holder Singleton<T>::mHolder;
template <class T>
T* Singleton<T>::mInstance = nullptr;

NAMESPACE_END
