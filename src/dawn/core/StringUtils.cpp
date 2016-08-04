/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "core/StringUtils.h"

namespace dw {
namespace str {

CommandLineArgs parseCommandArgs(int argc, char** argv) {
    CommandLineArgs args;

    // Parse arguments
    auto dispatchArgument = [&args](const String& name, const String& param) {
        if (name.length() > 0) {
            LOG << "\tName: " << name << " - Param: " << param;
            args.insert(make_pair(name, param));
        }
    };

    // Walk through argc/argv
    LOG << "Program Arguments:";
    String name, param;
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            dispatchArgument(name, param);
            name = String(argv[i]).substr(1);
            param.clear();
        } else {
            if (!param.empty())
                LOG << "Extra argument specified, ignoring";
            else
                param = String(argv[i]);
        }
    }

    // Dispatch final parameter
    dispatchArgument(name, param);

    return args;
}

String generateName(const String& prefix /*= "Unnamed"*/) {
    static Map<String, int> countMap;
    if (countMap.find(prefix) == countMap.end())
        countMap[prefix] = 0;
    return prefix + std::to_string(++countMap[prefix]);
}

String padString(const String& str, uint length) {
    String out = str;
    if (length > out.size())
        out.insert(out.size(), length - out.size(), ' ');
    return out;
}

Vector<String>& split(const String& s, char delim, Vector<String>& elems) {
    std::stringstream ss(s);
    String item;
    while (std::getline(ss, item, delim))
        elems.push_back(item);
    return elems;
}

String concat(const Vector<String>& vec, const String& delim) {
    String out;
    for (uint i = 0; i < vec.size() - 1; ++i)
        out += vec[i];
    if (!vec.empty())
        out += vec.back();
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
}

std::default_random_engine engine;

int randomInt(int min /*= 0*/, int max /*= INT_MAX*/) {
    return std::uniform_int_distribution<int>(min, max)(engine);
}

uint randomUnsigned(uint min /*= 0*/, uint max /*= UINT_MAX*/) {
    return std::uniform_int_distribution<uint>(min, max)(engine);
}

float randomFloat(float min /*= 0.0f*/, float max /*= 1.0f*/) {
    return std::uniform_real_distribution<float>(min, max)(engine);
}

double randomDouble(double min /*= 0.0f*/, double max /*= 1.0f*/) {
    return std::uniform_real_distribution<double>(min, max)(engine);
}

Vec2 randomVec2(const Vec2& min /*= Vec2(-1.0f)*/, const Vec2& max /*= Vec2(1.0f)*/) {
    return Vec2(std::uniform_real_distribution<float>(min.x, max.x)(engine),
                std::uniform_real_distribution<float>(min.y, max.y)(engine));
}

Vec3 randomVec3(const Vec3& min /*= Vec3(-1.0f)*/, const Vec3& max /*= Vec3(1.0f)*/) {
    return Vec3(std::uniform_real_distribution<float>(min.x, max.x)(engine),
                std::uniform_real_distribution<float>(min.y, max.y)(engine),
                std::uniform_real_distribution<float>(min.z, max.z)(engine));
}

Vec4 randomVec4(const Vec4& min /*= Vec4(-1.0f)*/, const Vec4& max /*= Vec4(1.0f)*/) {
    return Vec4(std::uniform_real_distribution<float>(min.x, max.x)(engine),
                std::uniform_real_distribution<float>(min.y, max.y)(engine),
                std::uniform_real_distribution<float>(min.z, max.z)(engine),
                std::uniform_real_distribution<float>(min.w, max.w)(engine));
}

namespace time {}
}
