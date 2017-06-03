/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
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
    do {
        read(&c, sizeof(char));
        out += c;
    } while (c != delim && !eof());
    return out;
}

bool InputStream::eof() const {
    return mPosition >= mSize;
}

u64 InputStream::getPosition() const {
    return mPosition;
}

u64 InputStream::getSize() const {
    return mSize;
}
}  // namespace dw
