/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

namespace dw {
using MemoryDeleter = void (*)(byte*);

// A blob of memory.
class DW_API Memory {
public:
    /// Constructs an empty memory block.
    Memory();

    /// Create an uninitialised memory block.
    explicit Memory(uint size);

    /// Creates a memory block by copying from existing data.
    /// Size in bytes.
    template <typename T> Memory(const T* data, uint size);

    /// Creates a memory block by copying from existing data.
    template <typename T> Memory(const Vector<T>& data);

    /// Takes ownership of an existing memory block, deleting it using the provided deleter.
    /// Size in bytes.
    template <typename T, typename Deleter> Memory(T* data, uint size, Deleter deleter);

    /// Destroys memory block.
    ~Memory() = default;

    // Copyable.
    Memory(const Memory&) = default;
    Memory& operator=(const Memory&) = default;

    // Movable.
    Memory(Memory&&) = default;
    Memory& operator=(Memory&&) = default;

    /// Access individual bytes.
    byte& operator[](size_t index) const;

    /// Access underlying byte buffer.
    byte* data() const;

    /// Size of the byte buffer.
    uint size() const;

private:
    SharedPtr<byte> data_;
    uint size_;
};

template <typename T> Memory::Memory(const T* data, uint size) : Memory(data ? size : 0) {
    if (data != nullptr) {
        memcpy(data_.get(), data, size);
    }
}

template <typename T>
Memory::Memory(const Vector<T>& data) : Memory(data.data(), data.size() * sizeof(T)) {
}

template <typename T, typename Deleter>
Memory::Memory(T* data, uint size, Deleter deleter) : size_{size} {
    data_.reset(static_cast<byte*>(static_cast<void*>(data)), deleter);
}
}  // namespace dw
