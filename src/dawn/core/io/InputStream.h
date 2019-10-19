/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "renderer/SystemPosition.h"
#include "core/math/Colour.h"

namespace dw {
class DW_API InputStream {
public:
    InputStream();
    explicit InputStream(usize size);
    virtual ~InputStream();

    /// Read an arbitrary amount of bytes from the stream.
    /// @return Number of bytes read
    virtual usize readData(void* dest, usize size) = 0;

    /// Reads a string up to a certain character
    /// @param delim Delimeter character
    String readLine(char delim = '\n');

    /// Read all bytes from the stream.
    /// @return A buffer containing all the bytes in the stream, or an error.
    Result<Vector<byte>> readAll();

    /// Moves the position of the cursor in the stream.
    /// @param position Offset from the start of the stream, in bytes
    virtual void seek(usize position) = 0;

    /// Check if the end of the stream has been reached
    bool eof() const;

    /// Returns the current position in the input stream
    usize position() const;

    /// Returns the size of the input stream
    usize size() const;

// Read for primitive types.
#define IMPL_PRIMITIVE_READ(T)       \
    virtual void read(T& value) {    \
        readData(&value, sizeof(T)); \
    }
    IMPL_PRIMITIVE_READ(i8)
    IMPL_PRIMITIVE_READ(u8)
    IMPL_PRIMITIVE_READ(i16)
    IMPL_PRIMITIVE_READ(u16)
    IMPL_PRIMITIVE_READ(i32)
    IMPL_PRIMITIVE_READ(u32)
    IMPL_PRIMITIVE_READ(i64)
    IMPL_PRIMITIVE_READ(u64)
    IMPL_PRIMITIVE_READ(char)
    IMPL_PRIMITIVE_READ(bool)
    IMPL_PRIMITIVE_READ(float)
    IMPL_PRIMITIVE_READ(double)

    // Read for common types.
    virtual void read(Vec3& v) {
        read(v.x);
        read(v.y);
        read(v.z);
    }

    virtual void read(Vec4& v) {
        read(v.x);
        read(v.y);
        read(v.z);
        read(v.w);
    }

    virtual void read(SystemPosition& p) {
        read(p.x);
        read(p.y);
        read(p.z);
    }

    virtual void read(Quat& q) {
        read(q.x);
        read(q.y);
        read(q.z);
        read(q.w);
    }

    virtual void read(String& s) {
        s = readLine('\0');
    }

    virtual void read(Colour& c) {
        read(c.rgba());
    }

protected:
    usize position_;
    usize size_;
};

namespace stream {
template <typename T> T read(InputStream& s) {
    T result;
    s.read(result);
    return result;
}
}  // namespace stream
}  // namespace dw
