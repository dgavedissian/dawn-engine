/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace dw {
using Thread = std::thread;

// Synchronisation primitives.
template <typename T> using Atomic = std::atomic<T>;
using Mutex = std::mutex;
template <typename T> using LockGuard = std::lock_guard<T>;
template <typename T> using UniqueLock = std::unique_lock<T>;
using ConditionVariable = std::condition_variable;

// Semaphore. Based on: https://stackoverflow.com/a/27852868
class Semaphore {
public:
    using native_handle_type = typename ConditionVariable::native_handle_type;

    explicit Semaphore(std::size_t count = 0);
    Semaphore(const Semaphore&) = delete;
    Semaphore(Semaphore&&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    Semaphore& operator=(Semaphore&&) = delete;

    void notify();
    void wait();
    bool try_wait();
    template <class Rep, class Period> bool wait_for(const std::chrono::duration<Rep, Period>& d);
    template <class Clock, class Duration>
    bool wait_until(const std::chrono::time_point<Clock, Duration>& t);

    native_handle_type native_handle();

private:
    Mutex mutex_;
    ConditionVariable cv_;
    std::size_t count_;
};


template<class Rep, class Period>
bool Semaphore::wait_for(const std::chrono::duration<Rep, Period> &d) {
    UniqueLock <Mutex> lock{mutex_};
    auto finished = cv_.wait_for(lock, d, [&] { return count_ > 0; });

    if (finished)
        --count_;

    return finished;
}

template<class Clock, class Duration>
bool Semaphore::wait_until(const std::chrono::time_point<Clock, Duration> &t) {
    UniqueLock <Mutex> lock{mutex_};
    auto finished = cv_.wait_until(lock, t, [&] { return count_ > 0; });

    if (finished)
        --count_;

    return finished;
}

// Barrier. Based on: https://stackoverflow.com/a/27118537
class Barrier {
public:
    explicit Barrier(std::size_t count) : threshold_(count), count_(count), generation_(0) {
    }

    void wait() {
        UniqueLock<Mutex> lock{mutex_};
        auto current_generation = generation_;
        if (--count_ == 0) {
            generation_++;
            count_ = threshold_;
            cv_.notify_all();
        } else {
            cv_.wait(lock, [this, current_generation] { return current_generation != generation_; });
        }
    }

private:
    Mutex mutex_;
    ConditionVariable cv_;
    std::size_t threshold_;
    std::size_t count_;
    std::size_t generation_;
};
}  // namespace dw