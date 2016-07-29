/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "ConfigNode.h"

#include <yaml-cpp/yaml.h>

namespace dw {

ConfigNode ConvertFromYaml(const YAML::Node& node)
{
    switch (node.Type())
    {
    case YAML::NodeType::Scalar:
        return ConfigNode(node.as<String>());

    case YAML::NodeType::Sequence:
        {
            ConfigNode seq;
            for (auto i = node.begin(); i != node.end(); i++)
            {
                const YAML::Node& n = *i;
                seq.push(ConvertFromYaml(n));
            }
            return seq;
        }

    case YAML::NodeType::Map:
        {
            ConfigNode m;
            for (auto i = node.begin(); i != node.end(); i++)
                m.insert(make_pair(i->first.as<String>(), ConvertFromYaml(i->second)));
            return m;
        }

    default:
        return ConfigNode();
    }
}

void EmitYaml(YAML::Emitter& out, const ConfigNode& node)
{
    switch (node.getType())
    {
    case NT_SCALAR:
        out << node.as<String>();
        break;

    case NT_SEQUENCE:
        out << YAML::BeginSeq;
        for (auto i = node.seq_begin(); i != node.seq_end(); i++)
            EmitYaml(out, *i);
        out << YAML::EndSeq;
        break;

    case NT_MAP:
        out << YAML::BeginMap;
        for (auto i = node.map_begin(); i != node.map_end(); i++)
        {
            out << YAML::Key << i->first << YAML::Value;
            EmitYaml(out, i->second);
        }
        out << YAML::EndMap;
        break;

    default:
        ERROR_WARN("Can't emit a YAML node of type " + std::to_string(node.getType()));
    }
}

std::istream& operator>>(std::istream& stream, ConfigNode &node)
{
    try
    {
        node = ConvertFromYaml(YAML::Load(stream));
    }
    catch (YAML::ParserException& e)
    {
        std::stringstream ss;
        ss << "YAML Parsing exception: " << e.what();
        ERROR_WARN(ss.str());
    }
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const ConfigNode& node)
{
    YAML::Emitter out;
    EmitYaml(out, node);
    return stream << out.c_str();
}

ConfigNode::ConfigNode() : mType(NT_NULL)
{
}

ConfigNode::ConfigNode(const ConfigNode& rhs) : mType(rhs.mType)
{
    mData.scalar = rhs.mData.scalar;
    mData.sequence = rhs.mData.sequence;
    mData.keymap = rhs.mData.keymap;
}

ConfigNode::~ConfigNode()
{
}

void ConfigNode::load(const String& s)
{
    std::istringstream(s) >> *this;
}

ConfigNodeType ConfigNode::getType() const
{
    return mType;
}

bool ConfigNode::isScalar() const
{
    return mType == NT_SCALAR;
}

bool ConfigNode::isSequence() const
{
    return mType == NT_SEQUENCE;
}

bool ConfigNode::isMap() const
{
    return mType == NT_MAP;
}

uint ConfigNode::size() const
{
    switch (mType)
    {
    case NT_SEQUENCE:
        return mData.sequence.size();

    case NT_MAP:
        return mData.keymap.size();

    default:
        return 0;
    }
}

void ConfigNode::push(const ConfigNode& v)
{
    if (mType == NT_NULL)
        mType = NT_SEQUENCE;
    mData.sequence.push_back(v);
}

ConfigNode& ConfigNode::operator[](uint i)
{
    assert(mType == NT_SEQUENCE);
    return mData.sequence[i];
}

const ConfigNode& ConfigNode::operator[](uint i) const
{
    assert(mType == NT_SEQUENCE);
    return mData.sequence[i];
}

Vector<ConfigNode>::iterator ConfigNode::seq_begin()
{
    return mData.sequence.begin();
}

Vector<ConfigNode>::const_iterator ConfigNode::seq_begin() const
{
    return mData.sequence.begin();
}

Vector<ConfigNode>::iterator ConfigNode::seq_end()
{
    return mData.sequence.end();
}

Vector<ConfigNode>::const_iterator ConfigNode::seq_end() const
{
    return mData.sequence.end();
}

void ConfigNode::insert(const Pair<String, ConfigNode>& p)
{
    if (mType == NT_NULL)
        mType = NT_MAP;
    mData.keymap.insert(p);
}

bool ConfigNode::contains(const String& key) const
{
    return mData.keymap.count(key) > 0;
}

ConfigNode& ConfigNode::operator[](const String& key)
{
    if (mType == NT_NULL)
        mType = NT_MAP;
    assert(mType == NT_MAP);
    return mData.keymap[key];
}

const ConfigNode& ConfigNode::operator[](const String& key) const
{
    assert(mType == NT_MAP);
    return mData.keymap.at(key);
}

Map<String, ConfigNode>::iterator ConfigNode::map_begin()
{
    return mData.keymap.begin();
}

Map<String, ConfigNode>::const_iterator ConfigNode::map_begin() const
{
    return mData.keymap.begin();
}

Map<String, ConfigNode>::iterator ConfigNode::map_end()
{
    return mData.keymap.end();
}

Map<String, ConfigNode>::const_iterator ConfigNode::map_end() const
{
    return mData.keymap.end();
}

}
