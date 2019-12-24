/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "CommandLine.h"

namespace dw {
CommandLine::CommandLine(int argc, char** argv) {
    for (int i = 0; i < argc; i++) {
        if (argv[i][0] == '-') {
            // Look ahead to next arg. If flag then add this arg as flag, otherwise add argument
            // pair
            if (i < argc - 1) {
                if (argv[i + 1][0] == '-') {
                    flags.insert(String{argv[i]});
                } else {
                    arguments[String{argv[i]}] = String{argv[i + 1]};
                    i++;  // skip argument value
                }
            } else {
                flags.insert(String{argv[i]});
            }
        }
    }
}
}  // namespace dw