/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "math/Vec2i.h"
#include "math/Vec3i.h"
#include "math/Vec4i.h"

namespace dw {

template <> struct Converter<bool> {
    static ConfigNode encode(const bool& value) {
        ConfigNode out;
        out.mType = NT_SCALAR;
        out.mData.scalar = value ? "true" : "false";
        return out;
    }

    static bool decode(const ConfigNode& node, bool& rhs) {
        if (node.mType != NT_SCALAR) return false;

        rhs = node.mData.scalar == "true";
        return true;
    }
};

template <> struct Converter<int> {
    static ConfigNode encode(const int& value) {
        ConfigNode out;
        out.mType = NT_SCALAR;
        out.mData.scalar = std::to_string(value);
        return out;
    }

    static bool decode(const ConfigNode& node, int& rhs) {
        if (node.mType != NT_SCALAR) return false;

        try {
            rhs = std::stoi(node.mData.scalar);
        } catch (std::exception& e) {
            LOG << "Unable to convert value " << node.mData.scalar << " - Reason: " << e.what();
            return false;
        }

        return true;
    }
};

template <> struct Converter<uint> {
    static ConfigNode encode(const uint& value) {
        ConfigNode out;
        out.mType = NT_SCALAR;
        out.mData.scalar = std::to_string(value);
        return out;
    }

    static bool decode(const ConfigNode& node, uint& rhs) {
        if (node.mType != NT_SCALAR) return false;

        try {
            rhs = std::stoul(node.mData.scalar);
        } catch (std::exception& e) {
            LOG << "Unable to convert value " << node.mData.scalar << " - Reason: " << e.what();
            return false;
        }

        return true;
    }
};

template <> struct Converter<uint64_t> {
    static ConfigNode encode(const uint64_t& value) {
        ConfigNode out;
        out.mType = NT_SCALAR;
        out.mData.scalar = std::to_string(value);
        return out;
    }

    static bool decode(const ConfigNode& node, uint64_t& rhs) {
        if (node.mType != NT_SCALAR) return false;

        try {
            rhs = std::stoull(node.mData.scalar);
        } catch (std::exception& e) {
            LOG << "Unable to convert value " << node.mData.scalar << " - Reason: " << e.what();
            return false;
        }

        return true;
    }
};

template <> struct Converter<float> {
    static ConfigNode encode(const float& value) {
        ConfigNode out;
        out.mType = NT_SCALAR;
        out.mData.scalar = std::to_string(value);
        return out;
    }

    static bool decode(const ConfigNode& node, float& rhs) {
        if (node.mType != NT_SCALAR) return false;

        rhs = std::stof(node.mData.scalar);
        return true;
    }
};

template <> struct Converter<double> {
    static ConfigNode encode(const double& value) {
        ConfigNode out;
        out.mType = NT_SCALAR;
        out.mData.scalar = std::to_string(value);
        return out;
    }

    static bool decode(const ConfigNode& node, double& rhs) {
        if (node.mType != NT_SCALAR) return false;

        rhs = std::stod(node.mData.scalar);
        return true;
    }
};

template <> struct Converter<String> {
    static ConfigNode encode(const String& value) {
        ConfigNode out;
        out.mType = NT_SCALAR;
        out.mData.scalar = value;
        return out;
    }

    static bool decode(const ConfigNode& node, String& rhs) {
        if (node.mType != NT_SCALAR) return false;

        rhs = node.mData.scalar;
        return true;
    }
};

template <> struct Converter<Vec2> {
    static ConfigNode encode(const Vec2& rhs) {
        ConfigNode node;
        node.push(rhs.x);
        node.push(rhs.y);
        return node;
    }

    static bool decode(const ConfigNode& node, Vec2& rhs) {
        if (!node.isSequence() || node.size() != 2) return false;

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        return true;
    }
};

template <> struct Converter<Vec2i> {
    static ConfigNode encode(const Vec2i& rhs) {
        ConfigNode node;
        node.push(rhs.x);
        node.push(rhs.y);
        return node;
    }

    static bool decode(const ConfigNode& node, Vec2i& rhs) {
        if (!node.isSequence() || node.size() != 2) return false;

        rhs.x = node[0].as<int>();
        rhs.y = node[1].as<int>();
        return true;
    }
};

template <> struct Converter<Vec3> {
    static ConfigNode encode(const Vec3& rhs) {
        ConfigNode node;
        node.push(rhs.x);
        node.push(rhs.y);
        node.push(rhs.z);
        return node;
    }

    static bool decode(const ConfigNode& node, Vec3& rhs) {
        if (!node.isSequence() || node.size() != 3) return false;

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        return true;
    }
};

template <> struct Converter<Vec3i> {
    static ConfigNode encode(const Vec3i& rhs) {
        ConfigNode node;
        node.push(rhs.x);
        node.push(rhs.y);
        node.push(rhs.z);
        return node;
    }

    static bool decode(const ConfigNode& node, Vec3i& rhs) {
        if (!node.isSequence() || node.size() != 3) return false;

        rhs.x = node[0].as<int>();
        rhs.y = node[1].as<int>();
        rhs.z = node[2].as<int>();
        return true;
    }
};

template <> struct Converter<Vec4> {
    static ConfigNode encode(const Vec4& rhs) {
        ConfigNode node;
        node.push(rhs.x);
        node.push(rhs.y);
        node.push(rhs.z);
        node.push(rhs.w);
        return node;
    }

    static bool decode(const ConfigNode& node, Vec4& rhs) {
        if (!node.isSequence() || node.size() != 4) return false;

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        rhs.w = node[3].as<float>();
        return true;
    }
};

template <> struct Converter<Vec4i> {
    static ConfigNode encode(const Vec4i& rhs) {
        ConfigNode node;
        node.push(rhs.x);
        node.push(rhs.y);
        node.push(rhs.z);
        node.push(rhs.w);
        return node;
    }

    static bool decode(const ConfigNode& node, Vec4i& rhs) {
        if (!node.isSequence() || node.size() != 4) return false;

        rhs.x = node[0].as<int>();
        rhs.y = node[1].as<int>();
        rhs.z = node[2].as<int>();
        rhs.w = node[3].as<int>();
        return true;
    }
};
}
