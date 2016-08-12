/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "io/OutputStream.h"

namespace dw {

OutputStream::~OutputStream() {
}

//
// Specialisations of stream::write
//
namespace stream {

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
