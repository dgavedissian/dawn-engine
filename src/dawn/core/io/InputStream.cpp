/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "InputStream.h"
#include <fmt/format.h>

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

Result<Vector<byte>> InputStream::readAll() {
    if (size_ == 0) {
        return makeError("Unable to read entire InputStream. Size is 0.");
    }
    Vector<byte> buffer;
    buffer.resize(size_);
    auto read = readData(buffer.data(), size_);
    if (!eof()) {
        return makeError(fmt::format(
            "Attempted to read {} bytes. Actually read {} bytes and didn't read EOF.", read));
    }
    return buffer;
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
