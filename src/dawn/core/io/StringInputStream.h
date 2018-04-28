/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "InputStream.h"

namespace dw {
class DW_API StringInputStream : public InputStream {
public:
    StringInputStream(const String& data);
    ~StringInputStream() = default;

    u32 readData(void* dest, u32 size) override;
    void seek(u64 position) override;

private:
    String data_;
};
}  // namespace dw
