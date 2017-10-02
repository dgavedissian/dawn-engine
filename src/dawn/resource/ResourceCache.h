/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "io/File.h"
#include "resource/Resource.h"

namespace dw {
class DW_API ResourceCache : public Object {
public:
    DW_OBJECT(ResourceCache);

    ResourceCache(Context* context);
    ~ResourceCache();

    void addResourceLocation(const Path& path);

    template <typename T> SharedPtr<T> get(const Path& filename) {
        String name(String(filename.c_str()));

        // If the resource already exists, return it.
        auto it = resource_cache_.find(name);
        if (it != resource_cache_.end()) {
            return staticPointerCast<T>((*it).second);
        }

        // Load the file which contains this resource data.
        SharedPtr<File> file = getFile(filename);
        if (!file) {
            log().error("Cannot find file %s", filename);
            return nullptr;
        }
        SharedPtr<T> resource = makeShared<T>(context());
        resource_cache_.emplace(name, resource);
        log().info("Loading asset '%s'", filename);
        resource->load(filename, *file.get());
        return resource;
    }

private:
    SharedPtr<File> getFile(const Path& filename);

    Vector<Path> resource_paths_;
    HashMap<String, SharedPtr<Resource>> resource_cache_;
};
}  // namespace dw
