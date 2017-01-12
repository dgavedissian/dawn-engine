/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

namespace dw {

class InputStream {
public:
    InputStream();
    InputStream(u64 size);
    virtual ~InputStream();

    /// Read an arbitrary amount of bytes from the stream.
    /// @return Number of bytes read
    virtual u32 read(void* dest, u32 size) = 0;

    /// Moves the position of the cursor in the stream.
    /// @param position Offset from the start of the stream, in bytes
    virtual void seek(u64 position) = 0;

    /// Check if the end of the stream has been reached
    bool eof();

    /// Reads a string up to a certain character
    /// @param delim Delimeter character
    String readLine(char delim = '\n');

    /// Returns the current position in the input stream
    u64 getPosition() const;

    /// Returns the size of the input stream
    u64 getSize() const;

protected:
    u64 mPosition;
    u64 mSize;
};

namespace stream {

template <class T> T read(InputStream& stream) {
    static_assert(sizeof(T) != sizeof(T), "stream::read is not implemented for arbitrary types");
    return T();
}

// Implement read for primitive types
#define IMPL_READ(T)                                     \
    template <> inline T read<T>(InputStream & stream) { \
        T value;                                         \
        stream.read(&value, sizeof(T));                  \
        return value;                                    \
    }

IMPL_READ(i8)
IMPL_READ(u8)
IMPL_READ(i16)
IMPL_READ(u16)
IMPL_READ(i32)
IMPL_READ(u32)
IMPL_READ(i64)
IMPL_READ(u64)
IMPL_READ(char)
IMPL_READ(bool)
IMPL_READ(float)
IMPL_READ(double)

/// Read a null terminated string
template <> inline String read<String>(InputStream& stream) {
    return stream.readLine('\0');
}
}
}
