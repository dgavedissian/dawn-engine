/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "core/Timer.h"

#include <iomanip>

namespace dw {
namespace time {

TimePoint beginTiming() {
    return _SteadyClock::now();
}

double elapsed(TimePoint then) {
    return elapsed(then, beginTiming());
}

double elapsed(TimePoint then, TimePoint now) {
    std::chrono::duration<double> duration = now - then;
    return duration.count();
}

SystemTimePoint now() {
    return _SystemClock::now();
}

String format(SystemTimePoint timepoint, const String& format) {
    StringStream out;
    std::time_t tt = _SystemClock::to_time_t(timepoint);
#if DW_PLATFORM == DW_WIN32
    tm time;
    errno_t err = ::gmtime_s(&time, &tt);
    if (err == EINVAL) {
        return "";
    }
#else
    tm time = *std::gmtime(&tt);
#endif
    out << std::put_time(&time, format.c_str());
    return out.str();
}
}  // namespace time
}  // namespace dw
