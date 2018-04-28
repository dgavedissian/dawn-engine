/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "core/StringUtils.h"

namespace dw {
namespace str {

CommandLineArgs parseCommandArgs(Logger& logger, int argc, char** argv) {
    CommandLineArgs args;

    // Parse arguments
    auto dispatchArgument = [&args, &logger](const String& name, const String& param) {
        if (name.length() > 0) {
            logger.info("\tName: %s - Param: %s", name, param);
            args.insert(make_pair(name, param));
        }
    };

    // Walk through argc/argv
    logger.info("Program Arguments:");
    String name, param;
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            dispatchArgument(name, param);
            name = String(argv[i]).substr(1);
            param.clear();
        } else {
            if (!param.empty()) {
                logger.info("Extra argument specified, ignoring");
            } else {
                param = String(argv[i]);
            }
        }
    }

    // Dispatch final parameter
    dispatchArgument(name, param);

    return args;
}

String generateName(const String& prefix /*= "Unnamed"*/) {
    static Map<String, int> countMap;
    if (countMap.find(prefix) == countMap.end()) {
        countMap[prefix] = 0;
    }
    return prefix + std::to_string(++countMap[prefix]);
}

String padString(const String& str, uint length) {
    String out = str;
    if (length > out.size()) {
        out.insert(out.size(), length - out.size(), ' ');
    }
    return out;
}

Vector<String>& split(const String& s, char delim, Vector<String>& elems) {
    std::stringstream ss(s);
    String item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

String join(const Vector<String>& vec, const String& delim) {
    String out;
    for (uint i = 0; i < vec.size() - 1; ++i) {
        out += vec[i] + delim;
    }
    if (!vec.empty()) {
        out += vec.back();
    }
    return out;
}

String replace(String subject, const String& search, const String& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return subject;
}
}  // namespace str
}  // namespace dw
