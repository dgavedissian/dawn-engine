/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "net/BitStream.h"

namespace dw {
InputBitStream::InputBitStream(const byte* data, usize length) : data_(data), length_(length) {
}

InputBitStream::InputBitStream(const Vector<byte>& data)
    : data_(data.data()), length_(data.size()) {
}

usize InputBitStream::readData(void* dest, usize size) {
    memcpy(dest, data_ + position_, size);
    position_ += size;
    return size;
}

void InputBitStream::seek(usize position) {
    position_ = position;
}

const byte* InputBitStream::data() const {
    return data_;
}

usize InputBitStream::length() const {
    return length_;
}

OutputBitStream::OutputBitStream(usize bytes_to_reserve) {
    data_.reserve(bytes_to_reserve);
}

usize OutputBitStream::writeData(const void* src, usize size) {
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

usize OutputBitStream::length() const {
    return data_.size();
}
}  // namespace dw
