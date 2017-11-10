/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "net/BitStream.h"

namespace dw {

BitStream::BitStream() {
}

BitStream::BitStream(const Vector<u8>& initial_data) : data_(initial_data) {
}

u32 BitStream::read(void* dest, u32 size) {
    memcpy(dest, data_.data() + position_, size);
    position_ += size;
    return size;
}

void BitStream::seek(u64 position) {
    position_ = position;
}

u32 BitStream::write(const void* src, u32 size) {
    size_t end = data_.size();
    data_.resize(data_.size() + size);
    memcpy(data_.data() + end, src, size);
    return size;
}

const Vector<u8> BitStream::data() const {
    return data_;
}
}  // namespace dw
