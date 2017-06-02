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
    Option(Option<T>&& other) noexcept;
    Option(const T& other);
    Option(T&& other);

    Option<T>& operator=(const Option<T>& other);
    Option<T>& operator=(Option<T>&& other) noexcept;
    Option<T>& operator=(const T& other);
    Option<T>& operator=(T&& other);

    bool isPresent() const;
    T& get();
    const T& get() const;

private:
    bool set_;
    byte data_[sizeof(T)];
};

template <class T> Option<T>::Option() : set_{false} {
}

template <class T> Option<T>::Option(const Option<T>& other) : set_{other.set_} {
    // Use copy constructor to initialise data_.
    if (set_) {
        new (data_) T(other.get());
    }
}

template <class T> Option<T>::Option(Option<T>&& other) noexcept : set_{other.set_} {
    // Use move constructor to initialise data_.
    if (set_) {
        new (data_) T(std::move(other.get()));
    }
}

template <class T> Option<T>::Option(const T& other) : set_{true} {
    new (data_) T(other);
}

template <class T> Option<T>::Option(T&& other) : set_{true} {
    new (data_) T(std::move(other));
}

template <class T> Option<T>& Option<T>::operator=(const Option<T>& other) {
    set_ = other.set_;
    get() = other.get();
    return *this;
}

template <class T> Option<T>& Option<T>::operator=(Option<T>&& other) noexcept {
    set_ = other.set_;
    get() = std::move(other.get());
    return *this;
}

template <class T> Option<T>& Option<T>::operator=(const T& other) {
    set_ = true;
    get() = other.get();
    return *this;
}

template <class T> Option<T>& Option<T>::operator=(T&& other) {
    set_ = true;
    get() = std::move(other.get());
    return *this;
}

template <class T> bool Option<T>::isPresent() const {
    return set_;
}

template <class T> T& Option<T>::get() {
    return *reinterpret_cast<T*>(data_);
}

template <class T> const T& Option<T>::get() const {
    return *reinterpret_cast<const T*>(data_);
}
}  // namespace dw
