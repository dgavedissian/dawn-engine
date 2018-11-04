/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "core/io/InputStream.h"
#include "core/io/OutputStream.h"

namespace dw {
// Note: Data must outlive the InputBitStream.
class DW_API InputBitStream : public InputStream {
public:
    InputBitStream(const byte* data, u32 length);
    InputBitStream(const Vector<byte>& data);
    ~InputBitStream() = default;

    // InputStream.
    u32 readData(void* dest, u32 size) override;
    void seek(u64 position) override;

    const byte* data() const;
    u32 length() const;

private:
    const byte* data_;
    u32 length_;
};

class DW_API OutputBitStream : public OutputStream {
public:
    OutputBitStream(u32 bytes_to_reserve = 0);
    ~OutputBitStream() = default;

    u32 writeData(const void* src, u32 size) override;

    const Vector<byte>& vec_data() const;

    const byte* data() const;
    u32 length() const;

private:
    Vector<byte> data_;
};
}  // namespace dw
