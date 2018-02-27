/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "io/InputStream.h"
#include "io/OutputStream.h"

namespace dw {
class DW_API InputBitStream : public InputStream {
public:
    InputBitStream(const Vector<u8>& data);
    ~InputBitStream() = default;

    // InputStream.
    u32 readData(void* dest, u32 size) override;
    void seek(u64 position) override;

    const Vector<u8>& data() const;

private:
    const Vector<u8>& data_;
};

class DW_API OutputBitStream : public OutputStream {
public:
    OutputBitStream(u32 bytes_to_reserve = 0);
    ~OutputBitStream() = default;

    u32 writeData(const void* src, u32 size) override;

    const Vector<u8>& data() const;

private:
    Vector<u8> data_;
};
}  // namespace dw
