/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "io/Path.h"
#include "resource/Resource.h"

namespace dw {
class DW_API ResourceCache : public Object {
public:
    DW_OBJECT(ResourceCache);

    ResourceCache(Context* context);
    ~ResourceCache();

    void addResourcePath(const Path& path);

    template <typename T> SharedPtr<T> get(const Path& filename) {
        String name(String(filename.c_str()));

        // If the resource already exists, cache hit
        auto it = mResourceCache.find(name);
        if (it != mResourceCache.end()) {
            return staticPointerCast<T>((*it).second);
        }

        // Get the file which contains this resource data
        SharedPtr<File> file = getFile(filename);
        SharedPtr<T> resource = makeShared<T>(getContext());
        mResourceCache.insert(makePair(name, resource));
        resource->load(*file.get());
        return resource;
    }

private:
    SharedPtr<File> getFile(const Path& filename);

    Vector<Path> mResourcePaths;
    HashMap<String, SharedPtr<Resource>> mResourceCache;
};
}
