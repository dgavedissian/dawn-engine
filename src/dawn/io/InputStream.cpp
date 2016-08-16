/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "io/InputStream.h"

namespace dw {

InputStream::InputStream() : mPosition(0), mSize(0) {
}

InputStream::InputStream(u64 size) : mPosition(0), mSize(size) {
}

InputStream::~InputStream() {
}

String InputStream::readLine(char delim) {
    String out;
    char c;
    read(&c, sizeof(char));
    while (c != delim && !eof()) {
        out += c;
        read(&c, sizeof(char));
    }
    return out;
}

bool InputStream::eof() {
    return mPosition >= mSize;
}

u64 InputStream::getSize() const {
    return mSize;
}

//
// Specialisations of stream::read
//

namespace stream {

// Implement read for primitive types
#define IMPL_READ(T)                          \
    template <> T read(InputStream& stream) { \
        T value;                              \
        stream.read(&value, sizeof(T));       \
        return value;                         \
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
template <> String read(InputStream& stream) {
    return stream.readLine('\0');
}
}
}
