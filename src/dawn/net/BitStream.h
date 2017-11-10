/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "io/InputStream.h"
#include "io/OutputStream.h"

namespace dw {
class DW_API BitStream : public InputStream, public OutputStream {
public:
    BitStream();
    BitStream(const Vector<u8>& initial_data);
    ~BitStream() = default;

    u32 read(void* dest, u32 size) override;
    void seek(u64 position) override;

    u32 write(const void* src, u32 size) override;

    const Vector<u8> data() const;

private:
    Vector<u8> data_;
};
}  // namespace dw
