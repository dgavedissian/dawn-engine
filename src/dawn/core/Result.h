/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "core/Collections.h"
#include "core/Option.h"

#include <iostream>

namespace dw {
struct None {};

template <typename T, typename E = String> class Result {
public:
    Result() = delete;
    Result(const T& value);
    Result(T&& value);
    Result(const E& error);
    Result(E&& error);

    /// Returns a Result with a value.
    static Result<T, E> success(const T& value);

    /// Returns a Result with a value.
    static Result<T, E> success(T&& value);

    /// Returns a Result with an error.
    static Result<T, E> failure(const E& error = {});

    /// Returns a Result with an error.
    static Result<T, E> failure(E&& error);

    /// Returns true if Result contains an error. */
    bool hasError() const;

    /// Returns true if Result contains a value, therefore doesn't have an error. */
    bool hasValue() const;

    /// Returns the value contained in Result if hasError is false. Otherwise, return a default value.
    const T& value(const T& default_value = T()) const;

    /// Returns the value contained in Result if hasError is false. It is a fatal error to call
    /// if HasError is true.
    const T& value() const;

    /// Returns the value contained in Result if hasError is false. It is a fatal error to call
    /// if HasError is true.
    T& value();

    /// Returns the error code contained in Result if hasError is true. It is a fatal error to call
    /// if HasError is false.
    const E& error() const;

    /// Returns true if Result contains a value
    explicit operator bool() const;

    /// Returns the value contained in Result if HasError is false. It is a fatal error to call
    /// if HasError is true.
    const T& operator*() const;

    /// Returns the value contained in Result if HasError is false. It is a fatal error to call
    /// if HasError is true.
    T& operator*();

    /// Returns the value contained in Result if HasError is false. It is a fatal error to call
    /// if HasError is true.
    const T* operator->() const;

    /// Returns the value contained in Result if HasError is false. It is a fatal error to call
    /// if HasError is true.
    T* operator->();

private:
    Option<T> value_;
    Option<E> error_;
};
}  // namespace dw

namespace dw {
template <typename T, typename E> Result<T, E>::Result(const T& value) : value_(value) {
}
template <typename T, typename E> Result<T, E>::Result(T&& value) : value_(std::move(value)) {
}
template <typename T, typename E> Result<T, E>::Result(const E& error) : error_(error) {
}
template <typename T, typename E> Result<T, E>::Result(E&& error) : error_(std::move(error)) {
}

template <typename T, typename E> Result<T, E> Result<T, E>::success(const T& value) {
    return Result<T, E>(value);
};

template <typename T, typename E> Result<T, E> Result<T, E>::success(T&& value) {
    return Result<T, E>(std::move(value));
};

template <typename T, typename E> Result<T, E> Result<T, E>::failure(const E& error) {
    return Result<T, E>(error);
};

template <typename T, typename E> Result<T, E> Result<T, E>::failure(E&& error) {
    return Result<T, E>(std::move(error));
};

template <typename T, typename E> bool Result<T, E>::hasError() const {
    return error_.isPresent();
}

template <typename T, typename E> bool Result<T, E>::hasValue() const {
    return value_.isPresent();
}

template <typename T, typename E> const T& Result<T, E>::value(const T& default_value) const
{
    return hasError() ? default_value : *value_;
}

template <typename T, typename E> const T& Result<T, E>::value() const {
    if (hasError()) {
        assert(!"getValue called when an error is present.");
        std::cerr << "getValue called when an error is present: " << *error_ << std::endl;
        std::terminate();
    }
    return *value_;
}

template <typename T, typename E> T& Result<T, E>::value() {
    if (hasError()) {
        std::cerr << "getValue called when an error is present: " << *error_ << std::endl;
        assert(!"getValue called when an error is present.");
        std::terminate();
    }
    return *value_;
}

template <typename T, typename E> const E& Result<T, E>::error() const {
    if (!hasError()) {
        assert(!"getError called when no error is present.");
        std::terminate();
    }
    return error_.get();
}

template <typename T, typename E> Result<T, E>::operator bool() const {
    return hasValue();
};

template <typename T, typename E> const T& Result<T, E>::operator*() const {
    return value();
};

template <typename T, typename E> T& Result<T, E>::operator*() {
    return value();
}

template <typename T, typename E> const T* Result<T, E>::operator->() const {
    return &value();
}

template <typename T, typename E> T* Result<T, E>::operator->() {
    return &value();
}

}  // namespace dw
