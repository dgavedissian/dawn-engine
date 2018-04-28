/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "net/BitStream.h"

namespace dw {
InputBitStream::InputBitStream(const Vector<u8>& data) : data_(data) {
}

u32 InputBitStream::readData(void* dest, u32 size) {
    memcpy(dest, data_.data() + position_, size);
    position_ += size;
    return size;
}

void InputBitStream::seek(u64 position) {
    position_ = position;
}

const Vector<u8>& InputBitStream::data() const {
    return data_;
}

OutputBitStream::OutputBitStream(u32 bytes_to_reserve) {
    data_.reserve(bytes_to_reserve);
}

u32 OutputBitStream::writeData(const void* src, u32 size) {
    size_t end = data_.size();
    data_.resize(data_.size() + size);
    memcpy(data_.data() + end, src, size);
    return size;
}

const Vector<u8>& OutputBitStream::data() const {
    return data_;
}
}  // namespace dw
