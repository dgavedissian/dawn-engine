/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "io/StringInputStream.h"

namespace dw {
StringInputStream::StringInputStream(const String& data)
    : InputStream{data.size() + 1}, data_{data} {
}

u32 StringInputStream::readData(void* dest, u32 size) {
    if (position_ + size > size_) {
        return 0;
    }
    memcpy(dest, &data_.c_str()[position_], size);
    position_ += size;
    return size;
}

void StringInputStream::seek(u64 position) {
    position_ = std::min(position, size_);
}
}  // namespace dw
