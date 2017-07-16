/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

namespace dw {
class Exception : public std::runtime_error {
public:
    Exception(const String& msg) : std::runtime_error{msg} {
    }
};
}  // namespace dw