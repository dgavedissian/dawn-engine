/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "ConfigNode.h"

NAMESPACE_BEGIN

// Global config file state
// TODO: Merge this into Engine class
struct DW_API Config
{
    // Load a configuration from a file
    static void load(const String& filename);

    // Save a configuration to a file
    static void save();

    // Dump the current config to the log
    static void dump();

    // Set a config value
    template <class T> static void set(const String& key, T value)
    {
        root[key] = value;
    }

    // Get a config value or create it if it doesn't exist
    template <class T> static T get(const String& key, T def = T())
    {
        if (root.contains(key))
        {
            return root[key].as<T>(def);
        }
        else
        {
            set(key, def);
            return def;
        }
    }

    // Root node
    static String filename;
    static ConfigNode root;
};

NAMESPACE_END
