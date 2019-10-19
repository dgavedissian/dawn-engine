/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "StringInputStream.h"

namespace dw {
StringInputStream::StringInputStream(const String& data)
    : InputStream{data.size() + 1}, data_{data} {
}

usize StringInputStream::readData(void* dest, usize size) {
    if (position_ + size > size_) {
        return 0;
    }
    memcpy(dest, &data_.c_str()[position_], size);
    position_ += size;
    return size;
}

void StringInputStream::seek(usize position) {
    position_ = std::min(position, size_);
}
}  // namespace dw
