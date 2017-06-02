/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "core/Timer.h"

namespace dw {
namespace time {

TimePoint beginTiming() {
    return _SteadyClock::now();
}

double elapsed(TimePoint then) {
    return elapsed(then, beginTiming());
}

double elapsed(TimePoint then, TimePoint now) {
    return std::chrono::duration_cast<std::chrono::seconds>(now - then).count();
}

SystemTimePoint now() {
    return _SystemClock::now();
}

String format(SystemTimePoint time, const String& format) {
    StringStream out;
    std::time_t tt = _SystemClock::to_time_t(time);
    out << std::put_time(std::gmtime(&tt), format.c_str());
    return out.str();
}
}  // namespace time
}  // namespace dw
