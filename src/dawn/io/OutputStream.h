/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "scene/Position.h"

namespace dw {

class DW_API OutputStream {
public:
    virtual ~OutputStream();

    /// Write an arbitrary amount of bytes to the stream.
    /// @return Number of bytes written
    virtual u32 write(const void* src, u32 size) = 0;
};

namespace stream {

/// Write functions implemented for different types
template <typename T> void write(OutputStream& s, const T& value) {
    static_assert(sizeof(T) != sizeof(T), "stream::write is not implemented for arbitrary types");
}

// Implement write for primitive types
#define IMPL_PRIMITIVE_WRITE(T)                                                    \
    template <> inline void write<T>(OutputStream & s, const T& value) { \
        s.write(&value, sizeof(T));                                      \
    }

IMPL_PRIMITIVE_WRITE(i8)
IMPL_PRIMITIVE_WRITE(u8)
IMPL_PRIMITIVE_WRITE(i16)
IMPL_PRIMITIVE_WRITE(u16)
IMPL_PRIMITIVE_WRITE(i32)
IMPL_PRIMITIVE_WRITE(u32)
IMPL_PRIMITIVE_WRITE(i64)
IMPL_PRIMITIVE_WRITE(u64)
IMPL_PRIMITIVE_WRITE(char)
IMPL_PRIMITIVE_WRITE(bool)
IMPL_PRIMITIVE_WRITE(float)
IMPL_PRIMITIVE_WRITE(double)

template <> inline void write<Position>(OutputStream& s, const Position& value) {
    stream::write(s, value.x);
    stream::write(s, value.y);
    stream::write(s, value.z);
}

template <> inline void write<Quat>(OutputStream& s, const Quat& value) {
    stream::write(s, value.x);
    stream::write(s, value.y);
    stream::write(s, value.z);
    stream::write(s, value.w);
}

/// Read from a null terminated string
template <> inline void write<String>(OutputStream& s, const String& str) {
    for (char c : str) {
        s.write(&c, sizeof(char));
    }
    char null_byte = '\0';
    s.write(&null_byte, sizeof(char));
}
}  // namespace stream
}  // namespace dw
