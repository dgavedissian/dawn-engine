/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "net/BitStream.h"

namespace dw {
InputBitStream::InputBitStream(const byte* data, u32 length) : data_(data), length_(length) {
}

InputBitStream::InputBitStream(const Vector<byte>& data)
    : data_(data.data()), length_(data.size()) {
}

u32 InputBitStream::readData(void* dest, u32 size) {
    memcpy(dest, data_ + position_, size);
    position_ += size;
    return size;
}

void InputBitStream::seek(u64 position) {
    position_ = position;
}

const byte* InputBitStream::data() const {
    return data_;
}

u32 InputBitStream::length() const {
    return length_;
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

const Vector<byte>& OutputBitStream::vec_data() const {
    return data_;
}

const byte* OutputBitStream::data() const {
    return data_.data();
}

u32 OutputBitStream::length() const {
    return data_.size();
}
}  // namespace dw
