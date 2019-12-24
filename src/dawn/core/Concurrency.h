/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <dga/semaphore.h>

namespace dw {
using Thread = std::thread;

// Synchronisation primitives.
template <typename T> using Atomic = std::atomic<T>;
using Mutex = std::mutex;
template <typename T> using LockGuard = std::lock_guard<T>;
template <typename T> using UniqueLock = std::unique_lock<T>;
using ConditionVariable = std::condition_variable;
using dga::Semaphore;
}  // namespace dw