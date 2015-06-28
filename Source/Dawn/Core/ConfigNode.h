/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

NAMESPACE_BEGIN

enum ConfigNodeType
{
    NT_NULL,
    NT_SCALAR,
    NT_SEQUENCE,
    NT_MAP
};

class ConfigNode;

struct ConfigNodeData
{
    string scalar;
    vector<ConfigNode> sequence;
    map<string, ConfigNode> keymap;
};

template <class T>
struct Converter
{
    static ConfigNode Encode(const T& value);
    static bool Decode(const ConfigNode& node, T& value);
};

class DW_API ConfigNode
{
public:
    template <class T> friend struct Converter;
    
    ConfigNode();
    ConfigNode(const ConfigNode& rhs);
    template <class T>
    ConfigNode(const T& s) : ConfigNode(Converter<T>::Encode(s)) {}
    ~ConfigNode();

    /// Load a configuration from a string
    /// @param s String to parse
    void Load(const string& s);

    // Stream operators
    friend std::istream& operator>>(std::istream& stream, ConfigNode &node);
    friend std::ostream& operator<<(std::ostream& stream, const ConfigNode& node);

    template <class T>
    T As() const
    {
        T out;
        Converter<T>::Decode(*this, out);
        return out;
    }

    template <class T>
    T As(const T& defaultValue) const
    {
        T out; 
        if (Converter<T>::Decode(*this, out))
            return out;
        else
            return defaultValue;
    }

    ConfigNodeType GetType() const;
    bool IsScalar() const;
    bool IsSequence() const;
    bool IsMap() const;
    uint Size() const;

    // Sequence operations
    void Push(const ConfigNode& v);
    ConfigNode& operator[](uint i);
    const ConfigNode& operator[](uint i) const;

    vector<ConfigNode>::iterator seq_begin();
    vector<ConfigNode>::const_iterator seq_begin() const;
    vector<ConfigNode>::iterator seq_end();
    vector<ConfigNode>::const_iterator seq_end() const;
    
    // Map operations
    void Insert(const pair<string, ConfigNode>& p);
    bool Contains(const string& key) const;
    ConfigNode& operator[](const string& key);
    const ConfigNode& operator[](const string& key) const;

    map<string, ConfigNode>::iterator map_begin();
    map<string, ConfigNode>::const_iterator map_begin() const;
    map<string, ConfigNode>::iterator map_end();
    map<string, ConfigNode>::const_iterator map_end() const;

private:
    ConfigNodeType mType;
    ConfigNodeData mData;
};

NAMESPACE_END
#include "ConfigNode.inl"
