/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
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

protected:
    u64 mPosition;
    u64 mSize;
};

namespace stream {

template <class T> T read(InputStream& stream) {
    // static_assert(false, "stream::read is not implemented for arbitrary types");
    return T();
}
}
}
