/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

namespace dw {

class OutputStream {
public:
    virtual ~OutputStream();

    /// Write an arbitrary amount of bytes to the stream.
    /// @return Number of bytes written
    virtual u32 write(const void* src, u32 size) = 0;
};

namespace stream {

/// Write functions implemented for different types
template <class T> void write(OutputStream& stream, const T& value) {
    // static_assert(false, "stream::write is not implemented for arbitrary types");
}

// Implement read for primitive types
#define IMPL_WRITE(T)                                              \
    template <> void write(OutputStream& stream, const T& value) { \
        stream.write(&value, sizeof(T));                           \
    }

IMPL_WRITE(i8)
IMPL_WRITE(u8)
IMPL_WRITE(i16)
IMPL_WRITE(u16)
IMPL_WRITE(i32)
IMPL_WRITE(u32)
IMPL_WRITE(i64)
IMPL_WRITE(u64)
IMPL_WRITE(char)
IMPL_WRITE(bool)
IMPL_WRITE(float)
IMPL_WRITE(double)

/// Read a null terminated string
template <> void write(OutputStream& stream, const String& str) {
    for (char c : str) {
        stream.write(&c, sizeof(char));
    }
    char nullByte = '\0';
    stream.write(&nullByte, sizeof(char));
}
}
}
