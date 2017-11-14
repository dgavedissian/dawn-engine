/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "io/Path.h"

namespace dw {
class DW_API FileSystem : public Subsystem {
public:
    DW_OBJECT(FileSystem);

    FileSystem(Context* context);
    ~FileSystem();

    bool setWorkingDir(const Path& path) const;
    Path workingDir() const;
    Path tempDir() const;

    bool fileExists(const Path& path) const;
    bool rename(const Path& oldname, const Path& newname) const;
    bool deleteFile(const Path& path) const;

    // TODO: list files in directory.
    // TODO: file walker.
};
}  // namespace dw
