/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

namespace dw {

class DW_API FileSystem : public Object {
public:
    DW_OBJECT(FileSystem);

    FileSystem(Context* context);
    ~FileSystem();

    bool setWorkingDir(const Path& path);
    Path getWorkingDir() const;

    bool fileExists(const Path& path);
};
}
