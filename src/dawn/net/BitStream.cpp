/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "io/BitStream.h"

#if DW_PLATFORM == DW_WIN32
#include <io.h>
#else
#include <sys/stat.h>
#endif

namespace dw {

BitStream::BitStream() {
}

u32 BitStream::read(void* dest, u32 size) {
    if (!handle_) {
        return 0;
    }

    size_t ret = fread(dest, size, 1, handle_);
    if (ret != 1) {
        // TODO(David): Correct error handling
        if (feof(handle_)) {
            ERROR_FATAL("Unexpected EOF reached");
        } else if (ferror(handle_)) {
            ERROR_FATAL("Unexpected error occurred");
        }
        return 0;
    }

    position_ += size;

    return size;
}

void BitStream::seek(u64 position) {
    if (!handle_) {
        return;
    }

    // If writing, then sparse seeks are allowed (where the cursor can be positioned past the end of
    // the file)
    if (!(mode_ & FileMode::Write) && position > size_) {
        position = size_;
    }

    fseek(handle_, static_cast<long>(position), SEEK_SET);
    position_ = position;
}

u32 BitStream::write(const void* src, u32 size) {
    if (!handle_) {
        return 0;
    }

    fwrite(src, size, 1, handle_);
    position_ += size;
    if (position_ > size_) {
        size_ = position_;
    }

    return size;
}
}  // namespace dw
