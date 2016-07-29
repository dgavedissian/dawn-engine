/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"

#include "Config.h"

dw::String dw::Config::filename;
dw::ConfigNode dw::Config::root;

namespace dw {

void Config::load(const String& filename)
{
    Config::filename = filename;
    InputFileStream file(filename);
    file >> Config::root;
    Config::dump();
}

void Config::save()
{
    OutputFileStream file(Config::filename);
    file << Config::root;
}

void Config::dump()
{
    LOG << Config::root;
}

}
