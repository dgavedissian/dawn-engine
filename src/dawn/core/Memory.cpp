/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "core/Memory.h"

namespace dw {
Memory::Memory() : Memory{nullptr, 0} {
}

Memory::Memory(const void* data, uint size) : data_{nullptr}, size_{size} {
    if (data != nullptr) {
        data_ = new byte[size];
        memcpy(data_, data, size);
    }
}

Memory::~Memory() {
    if (data_) {
        delete[] data_;
    }
}

Memory::Memory(const Memory& other) noexcept {
    *this = other;
}

Memory& Memory::operator=(const Memory& other) noexcept {
    if (other.data_ != nullptr) {
        data_ = new byte[other.size_];
        size_ = other.size_;
        memcpy(data_, other.data_, other.size_);
    } else {
        data_ = nullptr;
        size_ = 0;
    }
    return *this;
}

Memory::Memory(Memory&& other) noexcept {
    *this = std::move(other);
}

Memory& Memory::operator=(Memory&& other) noexcept {
    data_ = other.data_;
    size_ = other.size_;
    other.data_ = nullptr;
    other.size_ = 0;
    return *this;
}

void* Memory::data() const {
    return data_;
}

uint Memory::size() const {
    return size_;
}
}  // namespace dw