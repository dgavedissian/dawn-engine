/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "io/File.h"
#include "resource/Resource.h"

namespace dw {
using ResourcePath = String;

class DW_API ResourcePackage : public Object
{
public:
    DW_OBJECT(ResourcePackage);
};

class DW_API ResourceCache : public Object {
public:
    DW_OBJECT(ResourceCache);

    ResourceCache(Context* context);
    ~ResourceCache();

    void addPath(const ResourcePath& binding, const Path& path);
    void addPackage(const ResourcePath& binding, SharedPtr<ResourcePackage> package);

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

    using ResourcePackageBinding = Variant<Path, SharedPtr<ResourcePackage>>;

    Map<ResourcePath, ResourcePackageBinding> resource_location_bindings_;
    HashMap<String, SharedPtr<Resource>> resource_cache_;
};
}  // namespace dw
