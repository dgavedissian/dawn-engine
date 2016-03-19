/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"

dw::String dw::Config::filename;
dw::ConfigNode dw::Config::root;

NAMESPACE_BEGIN

void Config::load(const String& filename)
{
    Config::filename = filename;
    ifstream file(filename);
    file >> Config::root;
    Config::dump();
}

void Config::save()
{
    ofstream file(Config::filename);
    file << Config::root;
}

void Config::dump()
{
    LOG << Config::root;
}

NAMESPACE_END
