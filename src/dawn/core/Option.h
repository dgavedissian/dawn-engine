/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

namespace dw {

template <class T> class Option {
public:
    Option();
    Option(const Option<T>& other);
    Option(Option<T>&& other);
    Option(const T& other);
    Option(T&& other);

    Option<T>& operator=(const Option<T>& other);
    Option<T>& operator=(Option<T>&& other);
    Option<T>& operator=(const T& other);
    Option<T>& operator=(T&& other);

    bool isSet() const;
    T& get();
    const T& get() const;

private:
    bool mSetFlag;
    byte mData[sizeof(T)];
};

template <class T> Option<T>::Option() : mSetFlag(false) {
}

template <class T> Option<T>::Option(const Option<T>& other) : mSetFlag(other.mSetFlag) {
    // Use copy constructor to initialise mData
    if (mSetFlag) {
        new (mData) T(other.get());
    }
}

template <class T> Option<T>::Option(Option<T>&& other) : mSetFlag(other.mSetFlag) {
    // Use move constructor to initialise mData
    if (mSetFlag) {
        new (mData) T(std::move(other.get()));
    }
}

template <class T> Option<T>::Option(const T& other) : mSetFlag(true) {
    new (mData) T(other);
}

template <class T> Option<T>::Option(T&& other) : mSetFlag(true) {
    new (mData) T(std::move(other));
}

template <class T> Option<T>& Option<T>::operator=(const Option<T>& other) {
    mSetFlag = other.mSetFlag;
    get() = other.get();
}

template <class T> Option<T>& Option<T>::operator=(Option<T>&& other) {
    mSetFlag = other.mSetFlag;
    get() = std::move(other.get());
}

template <class T> Option<T>& Option<T>::operator=(const T& other) {
    mSetFlag = true;
    get() = other.get();
}

template <class T> Option<T>& Option<T>::operator=(T&& other) {
    mSetFlag = true;
    get() = std::move(other.get());
}

template <class T> bool Option<T>::isSet() const {
    return mSetFlag;
}

template <class T> T& Option<T>::get() {
    return *reinterpret_cast<T*>(mData);
}

template <class T> const T& Option<T>::get() const {
    return *reinterpret_cast<const T*>(mData);
}
}
