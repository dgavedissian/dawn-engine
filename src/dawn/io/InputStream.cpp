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
}
