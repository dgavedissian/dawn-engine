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

    /// Creates a memory block by moving from existing data (and taking ownership)
    template <typename T> Memory(Vector<T>&& data);

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
    // A type erased pointer which owns an object that holds some data.
    SharedPtr<void> holder_;
};

template <typename T> Memory::Memory(const T* data, uint size) : Memory(data ? size : 0) {
    if (data != nullptr) {
        memcpy(data_.get(), data, size);
    }
}

template <typename T>
Memory::Memory(const Vector<T>& data) : Memory(data.data(), data.size() * sizeof(T)) {
}

template<typename T>
Memory::Memory(Vector<T>&& data) {
    SharedPtr<Vector<T>> vector_holder = makeShared<Vector<T>>(std::move(data));
    // SharedPtr shouldn't own any memory, the memory is owned by vector_holder. As a result, we pass a no-op deleter.
    data_ = SharedPtr<byte>(static_cast<byte*>(static_cast<void*>(vector_holder->data())), [](byte*) {});
    size_ = vector_holder->size() * sizeof(T);
    holder_ = vector_holder;
}

template <typename T, typename Deleter>
Memory::Memory(T* data, uint size, Deleter deleter) : size_{size} {
    data_.reset(static_cast<byte*>(static_cast<void*>(data)), deleter);
}
}  // namespace dw
