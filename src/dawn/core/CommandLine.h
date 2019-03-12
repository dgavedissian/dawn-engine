/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#pragma once

namespace dw {
struct DW_API CommandLine {
    CommandLine() = default;
    CommandLine(int argc, char** argv);

    Set<String> flags;
    HashMap<String, String> arguments;
};
}  // namespace dw