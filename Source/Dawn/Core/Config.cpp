/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#include "Common.h"

dw::string dw::Config::filename;
dw::ConfigNode dw::Config::root;

NAMESPACE_BEGIN

void Config::Load(const string& filename)
{
    Config::filename = filename;
    ifstream file(filename);
    file >> Config::root;
    Config::Dump();
}

void Config::Save()
{
    ofstream file(Config::filename);
    file << Config::root;
}

void Config::Dump()
{
    LOG << Config::root;
}

NAMESPACE_END
