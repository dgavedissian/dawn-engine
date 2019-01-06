/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "InputStream.h"

namespace dw {

InputStream::InputStream() : position_(0), size_(0) {
}

InputStream::InputStream(usize size) : position_(0), size_(size) {
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

usize InputStream::position() const {
    return position_;
}

usize InputStream::size() const {
    return size_;
}
}  // namespace dw
