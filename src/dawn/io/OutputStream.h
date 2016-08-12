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
}
}
