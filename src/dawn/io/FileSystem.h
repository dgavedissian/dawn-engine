/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
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

    static const u32 MAX_PATH;

};

}