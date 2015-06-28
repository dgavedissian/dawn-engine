/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

#include "ConfigNode.h"

NAMESPACE_BEGIN

// Global config file state
// TODO: Merge this into Engine class
struct DW_API Config
{
    // Load a configuration from a file
    static void Load(const string& filename);

    // Save a configuration to a file
    static void Save();

    // Dump the current config to the log
    static void Dump();

    // Set a config value
    template <class T> static void Set(const string& key, T value)
    {
        root[key] = value;
    }

    // Get a config value or create it if it doesn't exist
    template <class T> static T Get(const string& key, T def = T())
    {
        if (root.Contains(key))
        {
            return root[key].As<T>(def);
        }
        else
        {
            Set(key, def);
            return def;
        }
    }

    // Root node
    static string filename;
    static ConfigNode root;
};

NAMESPACE_END
