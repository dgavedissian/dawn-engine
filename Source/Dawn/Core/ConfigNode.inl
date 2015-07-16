/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

NAMESPACE_BEGIN

template <> struct Converter<bool>
{
    static ConfigNode Encode(const bool& value)
    {
        ConfigNode out;
        out.mType = NT_SCALAR;
        out.mData.scalar = value ? "true" : "false";
        return out;
    }

    static bool Decode(const ConfigNode& node, bool& rhs)
    {
        if (node.mType != NT_SCALAR)
            return false;

        rhs = node.mData.scalar == "true";
        return true;
    }
};

template <> struct Converter<int>
{
    static ConfigNode Encode(const int& value)
    {
        ConfigNode out;
        out.mType = NT_SCALAR;
        out.mData.scalar = std::to_string(value);
        return out;
    }

    static bool Decode(const ConfigNode& node, int& rhs)
    {
        if (node.mType != NT_SCALAR)
            return false;

        try
        {
            rhs = std::stoi(node.mData.scalar);
        }
        catch (std::exception& e)
        {
            LOG << "Unable to convert value " << node.mData.scalar << " - Reason: " << e.what();
            return false;
        }

        return true;
    }
};

template <> struct Converter<uint>
{
    static ConfigNode Encode(const uint& value)
    {
        ConfigNode out;
        out.mType = NT_SCALAR;
        out.mData.scalar = std::to_string(value);
        return out;
    }

    static bool Decode(const ConfigNode& node, uint& rhs)
    {
        if (node.mType != NT_SCALAR)
            return false;

        try
        {
            rhs = std::stoul(node.mData.scalar);
        }
        catch (std::exception& e)
        {
            LOG << "Unable to convert value " << node.mData.scalar << " - Reason: " << e.what();
            return false;
        }

        return true;
    }
};

template <> struct Converter<uint64_t>
{
    static ConfigNode Encode(const uint64_t& value)
    {
        ConfigNode out;
        out.mType = NT_SCALAR;
        out.mData.scalar = std::to_string(value);
        return out;
    }

    static bool Decode(const ConfigNode& node, uint64_t& rhs)
    {
        if (node.mType != NT_SCALAR)
            return false;

        try
        {
            rhs = std::stoull(node.mData.scalar);
        }
        catch (std::exception& e)
        {
            LOG << "Unable to convert value " << node.mData.scalar << " - Reason: " << e.what();
            return false;
        }

        return true;
    }
};

template <> struct Converter<float>
{
    static ConfigNode Encode(const float& value)
    {
        ConfigNode out;
        out.mType = NT_SCALAR;
        out.mData.scalar = std::to_string(value);
        return out;
    }

    static bool Decode(const ConfigNode& node, float& rhs)
    {
        if (node.mType != NT_SCALAR)
            return false;

        rhs = std::stof(node.mData.scalar);
        return true;
    }
};

template <> struct Converter<double>
{
    static ConfigNode Encode(const double& value)
    {
        ConfigNode out;
        out.mType = NT_SCALAR;
        out.mData.scalar = std::to_string(value);
        return out;
    }

    static bool Decode(const ConfigNode& node, double& rhs)
    {
        if (node.mType != NT_SCALAR)
            return false;

        rhs = std::stod(node.mData.scalar);
        return true;
    }
};

template <> struct Converter<String>
{
    static ConfigNode Encode(const String& value)
    {
        ConfigNode out;
        out.mType = NT_SCALAR;
        out.mData.scalar = value;
        return out;
    }

    static bool Decode(const ConfigNode& node, String& rhs)
    {
        if (node.mType != NT_SCALAR)
            return false;

        rhs = node.mData.scalar;
        return true;
    }
};

template <> struct Converter<Vec2>
{
    static ConfigNode Encode(const Vec2& rhs)
    {
        ConfigNode node;
        node.Push(rhs.x);
        node.Push(rhs.y);
        return node;
    }

    static bool Decode(const ConfigNode& node, Vec2& rhs)
    {
        if (!node.IsSequence() || node.Size() != 2)
            return false;

        rhs.x = node[0].As<float>();
        rhs.y = node[1].As<float>();
        return true;
    }
};

template <> struct Converter<Vec2i>
{
    static ConfigNode Encode(const Vec2i& rhs)
    {
        ConfigNode node;
        node.Push(rhs.x);
        node.Push(rhs.y);
        return node;
    }

    static bool Decode(const ConfigNode& node, Vec2i& rhs)
    {
        if (!node.IsSequence() || node.Size() != 2)
            return false;

        rhs.x = node[0].As<int>();
        rhs.y = node[1].As<int>();
        return true;
    }
};

template <> struct Converter<Vec3>
{
    static ConfigNode Encode(const Vec3& rhs)
    {
        ConfigNode node;
        node.Push(rhs.x);
        node.Push(rhs.y);
        node.Push(rhs.z);
        return node;
    }

    static bool Decode(const ConfigNode& node, Vec3& rhs)
    {
        if (!node.IsSequence() || node.Size() != 3)
            return false;

        rhs.x = node[0].As<float>();
        rhs.y = node[1].As<float>();
        rhs.z = node[2].As<float>();
        return true;
    }
};

template <> struct Converter<Vec3i>
{
    static ConfigNode Encode(const Vec3i& rhs)
    {
        ConfigNode node;
        node.Push(rhs.x);
        node.Push(rhs.y);
        node.Push(rhs.z);
        return node;
    }

    static bool Decode(const ConfigNode& node, Vec3i& rhs)
    {
        if (!node.IsSequence() || node.Size() != 3)
            return false;

        rhs.x = node[0].As<int>();
        rhs.y = node[1].As<int>();
        rhs.z = node[2].As<int>();
        return true;
    }
};

template <> struct Converter<Vec4>
{
    static ConfigNode Encode(const Vec4& rhs)
    {
        ConfigNode node;
        node.Push(rhs.x);
        node.Push(rhs.y);
        node.Push(rhs.z);
        node.Push(rhs.w);
        return node;
    }

    static bool Decode(const ConfigNode& node, Vec4& rhs)
    {
        if (!node.IsSequence() || node.Size() != 4)
            return false;

        rhs.x = node[0].As<float>();
        rhs.y = node[1].As<float>();
        rhs.z = node[2].As<float>();
        rhs.w = node[3].As<float>();
        return true;
    }
};

template <> struct Converter<Vec4i>
{
    static ConfigNode Encode(const Vec4i& rhs)
    {
        ConfigNode node;
        node.Push(rhs.x);
        node.Push(rhs.y);
        node.Push(rhs.z);
        node.Push(rhs.w);
        return node;
    }

    static bool Decode(const ConfigNode& node, Vec4i& rhs)
    {
        if (!node.IsSequence() || node.Size() != 4)
            return false;

        rhs.x = node[0].As<int>();
        rhs.y = node[1].As<int>();
        rhs.z = node[2].As<int>();
        rhs.w = node[3].As<int>();
        return true;
    }
};

NAMESPACE_END
