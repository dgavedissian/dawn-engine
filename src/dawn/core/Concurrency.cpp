/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "core/Concurrency.h"

namespace dw {
Semaphore::Semaphore(std::size_t count) : count_{count} {
}

void Semaphore::notify() {
    LockGuard<Mutex> lock{mutex_};
    ++count_;
    cv_.notify_one();
}

void Semaphore::wait() {
    UniqueLock<Mutex> lock{mutex_};
    cv_.wait(lock, [&] { return count_ > 0; });
    --count_;
}

bool Semaphore::try_wait() {
    LockGuard<Mutex> lock{mutex_};

    if (count_ > 0) {
        --count_;
        return true;
    }

    return false;
}

Semaphore::native_handle_type Semaphore::native_handle() {
    return cv_.native_handle();
}
}  // namespace dw