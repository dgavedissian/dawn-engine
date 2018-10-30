/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "core/Memory.h"
#include "Memory.h"

namespace dw {
Memory::Memory() : data_{nullptr}, size_{0} {
}

Memory::Memory(uint size) : size_{size} {
    if (size > 0) {
        data_.reset(new byte[size], std::default_delete<byte[]>());
    }
}

byte& Memory::operator[](size_t index) const {
    return data_.get()[index];
}

byte* Memory::data() const {
    return data_.get();
}

uint Memory::size() const {
    return size_;
}
}  // namespace dw
