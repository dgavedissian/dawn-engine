/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "resource/Resource.h"

namespace dw {

class ResourceCache : public Object {
public:
    DW_OBJECT(ResourceCache);

    ResourceCache(Context* context);
    ~ResourceCache();

    void addResourcePath(const Path& path);

    template <class T> T* getResource(const Path& filename) {
        StringHash nameHash(String(filename.c_str()));

        // If the resource already exists, cache hit
        auto it = mResourceCache.find(nameHash);
        if (it != mResourceCache.end()) {
            return static_cast<T*>((*it).second.get());
        }

        // Get the file which contains this resource data
        SharedPtr<File> file = getFile(filename);
        SharedPtr<T> resource = makeShared<T>(getContext());
        mResourceCache.insert(makePair(nameHash, resource));
        resource->load(*file.get());
        return resource.get();
    }

private:
    SharedPtr<File> getFile(const Path& filename);

    Vector<Path> mResourcePaths;
    HashMap<StringHash, SharedPtr<Resource>> mResourceCache;
};
}