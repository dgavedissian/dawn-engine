/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "io/InputStream.h"

namespace dw {

InputStream::InputStream() : position_(0), size_(0) {
}

InputStream::InputStream(u64 size) : position_(0), size_(size) {
}

InputStream::~InputStream() {
}

String InputStream::readLine(char delim) {
    String out;
    char c;
    while (!eof()) {
        readData(&c, sizeof(char));
        if (c == delim) {
            break;
        }
        out += c;
    }
    return out;
}

bool InputStream::eof() const {
    return position_ >= size_;
}

u64 InputStream::position() const {
    return position_;
}

u64 InputStream::size() const {
    return size_;
}
}  // namespace dw
