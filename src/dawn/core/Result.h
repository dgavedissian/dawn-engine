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

    /// Returns the value contained in Result if HasError is false. It is a fatal error to call
    /// if HasError is true.
    const T& getValue() const;

    /// Returns the value contained in Result if HasError is false. It is a fatal error to call
    /// if HasError is true.
    T& getValue();

    /// Returns the error code contained in Result if HasError is true. It is a fatal error to call
    /// if HasError is false.
    const E& getError() const;

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
    Option<T> value;
    Option<E> error;
};
}  // namespace dw

namespace dw {
template <typename T, typename E> Result<T, E>::Result(const T& value) : value(value) {
}
template <typename T, typename E> Result<T, E>::Result(T&& value) : value(std::move(value)) {
}
template <typename T, typename E> Result<T, E>::Result(const E& error) : error(error) {
}
template <typename T, typename E> Result<T, E>::Result(E&& error) : error(std::move(error)) {
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
    return error.isPresent();
}

template <typename T, typename E> bool Result<T, E>::hasValue() const {
    return value.isPresent();
}

template <typename T, typename E> const T& Result<T, E>::getValue() const {
    if (hasError()) {
        assert(!"getValue called when an error is present.");
        std::cerr << "getValue called when an error is present: " << *error << std::endl;
        std::terminate();
    }
    return *value;
}

template <typename T, typename E> T& Result<T, E>::getValue() {
    if (hasError()) {
        std::cerr << "getValue called when an error is present: " << *error << std::endl;
        assert(!"getValue called when an error is present.");
        std::terminate();
    }
    return *value;
}

template <typename T, typename E> const E& Result<T, E>::getError() const {
    if (!hasError()) {
        assert(!"getError called when no error is present.");
        std::terminate();
    }
    return error.get();
}

template <typename T, typename E> Result<T, E>::operator bool() const {
    return hasValue();
};

template <typename T, typename E> const T& Result<T, E>::operator*() const {
    return getValue();
};

template <typename T, typename E> T& Result<T, E>::operator*() {
    return getValue();
}

template <typename T, typename E> const T* Result<T, E>::operator->() const {
    return &getValue();
}

template <typename T, typename E> T* Result<T, E>::operator->() {
    return &getValue();
}

}  // namespace dw