/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

namespace dw {

// A blob of memory.
class DW_API Memory {
public:
    /// Constructs an empty memory block.
    Memory();

    /// Copy data.
    Memory(const void* data, uint size);

    /// Destroys memory block.
    ~Memory();

    // Copyable.
    Memory(const Memory&) noexcept;
    Memory& operator=(const Memory&) noexcept;

    // Movable.
    Memory(Memory&&) noexcept;
    Memory& operator=(Memory&&) noexcept;

    void* data() const;
    uint size() const;

private:
    byte* data_;
    uint size_;
};

}  // namespace dw