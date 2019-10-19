/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "core/io/InputStream.h"
#include "core/io/OutputStream.h"

namespace dw {
// Note: Data must outlive the InputBitStream.
class DW_API InputBitStream : public InputStream {
public:
    InputBitStream(const byte* data, usize length);
    InputBitStream(const Vector<byte>& data);
    ~InputBitStream() = default;

    // InputStream.
    usize readData(void* dest, usize size) override;
    void seek(usize position) override;

    const byte* data() const;
    usize length() const;

private:
    const byte* data_;
    usize length_;
};

class DW_API OutputBitStream : public OutputStream {
public:
    OutputBitStream(usize bytes_to_reserve = 0);
    ~OutputBitStream() = default;

    usize writeData(const void* src, usize size) override;

    const Vector<byte>& vec_data() const;

    const byte* data() const;
    usize length() const;

private:
    Vector<byte> data_;
};
}  // namespace dw
