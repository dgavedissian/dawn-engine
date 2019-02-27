/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#pragma once

#include <chrono>

namespace dw {
namespace time {

using _SystemClock = std::chrono::system_clock;
using _SteadyClock = std::chrono::high_resolution_clock;
using TimePoint = _SteadyClock::time_point;
using SystemTimePoint = _SystemClock::time_point;
template <class T> using Duration = std::chrono::duration<double, T>;

using nano = std::nano;
using micro = std::micro;
using milli = std::milli;
using seconds = std::ratio<1>;
using minutes = std::ratio<60>;
using hours = std::ratio<3600>;

// Intervals

// TODO(David): Use a RAII class here?!

// Return the current steady time to be used for interval calculations
DW_API TimePoint beginTiming();

// Return the time elapsed in seconds since a time point to when this function was called
DW_API double elapsed(TimePoint then);

// TODO(David) comment
DW_API double elapsed(TimePoint then, TimePoint now);

// Calendar

// return the current system time
DW_API SystemTimePoint now();

// Format a timepoint as a string
DW_API String format(SystemTimePoint time, const String& format);
}  // namespace time
}  // namespace dw
