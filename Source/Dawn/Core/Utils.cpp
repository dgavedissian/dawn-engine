/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#include "Common.h"

NAMESPACE_BEGIN

CommandLineArgs ParseCommandArgs(int argc, char** argv)
{
    CommandLineArgs args;

    // Parse arguments
    auto dispatchArgument = [&args](const String& name, const String& param)
    {
        if (name.length() > 0)
        {
            LOG << "\tName: " << name << " - Param: " << param;
            args.insert(make_pair(name, param));
        }
    };

    // Walk through argc/argv
    LOG << "Program Arguments:";
    String name, param;
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            dispatchArgument(name, param);
            name = String(argv[i]).substr(1);
            param.clear();
        }
        else
        {
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

String GenerateName(const String& prefix /*= "Unnamed"*/)
{
    static Map<String, int> countMap;
    if (countMap.find(prefix) == countMap.end())
        countMap[prefix] = 0;
    return prefix + std::to_string(++countMap[prefix]);
}

String PadString(const String& str, uint length)
{
    String out = str;
    if (length > out.size())
        out.insert(out.size(), length - out.size(), ' ');
    return out;
}

Vector<String>& Split(const String& s, char delim, Vector<String>& elems)
{
    std::stringstream ss(s);
    String item;
    while (std::getline(ss, item, delim))
        elems.push_back(item);
    return elems;
}

String Concat(const Vector<String>& vec, const String& delim)
{
    String out;
    for (uint i = 0; i < vec.size() - 1; ++i)
        out += vec[i];
    if (!vec.empty())
        out += vec.back();
    return out;
}

std::default_random_engine engine;

int RandomInt(int min /*= 0*/, int max /*= INT_MAX*/)
{
    return std::uniform_int_distribution<int>(min, max)(engine);
}

uint RandomUint(uint min /*= 0*/, uint max /*= UINT_MAX*/)
{
    return std::uniform_int_distribution<uint>(min, max)(engine);
}

float RandomFloat(float min /*= 0.0f*/, float max /*= 1.0f*/)
{
    return std::uniform_real_distribution<float>(min, max)(engine);
}

double RandomDouble(double min /*= 0.0f*/, double max /*= 1.0f*/)
{
    return std::uniform_real_distribution<double>(min, max)(engine);
}

Vec2 RandomVec2(const Vec2& min /*= Vec2(-1.0f)*/, const Vec2& max /*= Vec2(1.0f)*/)
{
    return Vec2(std::uniform_real_distribution<float>(min.x, max.x)(engine),
                std::uniform_real_distribution<float>(min.y, max.y)(engine));
}

Vec3 RandomVec3(const Vec3& min /*= Vec3(-1.0f)*/, const Vec3& max /*= Vec3(1.0f)*/)
{
    return Vec3(std::uniform_real_distribution<float>(min.x, max.x)(engine),
                std::uniform_real_distribution<float>(min.y, max.y)(engine),
                std::uniform_real_distribution<float>(min.z, max.z)(engine));
}

Vec4 RandomVec4(const Vec4& min /*= Vec4(-1.0f)*/, const Vec4& max /*= Vec4(1.0f)*/)
{
    return Vec4(std::uniform_real_distribution<float>(min.x, max.x)(engine),
                std::uniform_real_distribution<float>(min.y, max.y)(engine),
                std::uniform_real_distribution<float>(min.z, max.z)(engine),
                std::uniform_real_distribution<float>(min.w, max.w)(engine));
}

namespace time
{
    TimePoint Now()
    {
        static Ogre::Timer timer;
        return static_cast<TimePoint>(timer.getMicroseconds()) * 1e-6;
    }

    Duration GetElapsed(TimePoint tp)
    {
        return Now() - tp;
    }

    String Format(time_t time, const String& formatString)
    {
#if DW_PLATFORM == DW_WIN32
        std::stringstream out;
        out << std::put_time(std::gmtime(&time), formatString.c_str());
        return out.str();
#else
        return "";
#endif
    }
}

NAMESPACE_END
