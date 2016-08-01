/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

namespace dw {
namespace time {

using _SystemClock = std::chrono::system_clock;
using _SteadyClock = std::chrono::steady_clock;
using TimePoint = _SteadyClock::time_point;
using SystemTimePoint = _SystemClock::time_point;
template <class T> using Duration = std::chrono::duration<double, std::chrono::seconds>;

using std::chrono::nanoseconds;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::minutes;
using std::chrono::hours;

// Intervals

// Return the current steady time to be used for interval calculations
DW_API TimePoint beginTiming();

// Return the time elapsed in seconds since a time point to when this function was called
DW_API double elapsed(TimePoint then);

// Calendar

// return the current system time
DW_API SystemTimePoint now();

// Format a timepoint as a string
DW_API String format(SystemTimePoint time, const String& format);
}
}
