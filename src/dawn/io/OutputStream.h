/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

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
#define IMPL_WRITE(T)                                                    \
    template <> inline void write<T>(OutputStream & s, const T& value) { \
        s.write(&value, sizeof(T));                                      \
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
