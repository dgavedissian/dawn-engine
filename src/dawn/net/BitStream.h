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
    DW_OBJECT(BitStream);

    BitStream();
    ~BitStream() = default;

    u32 read(void* dest, u32 size) override;
    void seek(u64 position) override;

    u32 write(const void* src, u32 size) override;

    bool open(const Path& path, int mode);
    void close();

private:
    Vector<uint8> bytes;
};
}  // namespace dw
