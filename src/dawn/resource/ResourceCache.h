/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "io/File.h"
#include "resource/Resource.h"

namespace dw {
using ResourcePath = String;

class DW_API ResourceLocation {
public:
    virtual ~ResourceLocation() = default;
    virtual SharedPtr<InputStream> getFile(const ResourcePath& path_within_location) = 0;
};

class DW_API ResourcePackage : public Object, public ResourceLocation
{
public:
    DW_OBJECT(ResourcePackage);

    ResourcePackage(Context* ctx, const Path& package);

    SharedPtr<InputStream> getFile(const ResourcePath& path_within_location) override;
};

class DW_API ResourceFilesystemPath : public Object, public ResourceLocation {
public:
    DW_OBJECT(ResourceFilesystemPath);

    ResourceFilesystemPath(Context* ctx, const Path& path);

    SharedPtr<InputStream> getFile(const ResourcePath& path_within_location) override;

private:
    Path path_;
};

class DW_API ResourceCache : public Object {
public:
    DW_OBJECT(ResourceCache);

    ResourceCache(Context* context);
    ~ResourceCache();

    void addPath(const ResourcePath& binding, const Path& path);
    void addPackage(const ResourcePath& binding, UniquePtr<ResourcePackage>&& package);

    template <typename T> SharedPtr<T> get(const Path& filename) {
        String name(String(filename.c_str()));

        // If the resource already exists, return it.
        auto it = resource_cache_.find(name);
        if (it != resource_cache_.end()) {
            return staticPointerCast<T>((*it).second);
        }

        // Load the file which contains this resource data.
        SharedPtr<InputStream> resource_data = getResourceData(filename);
        if (!resource_data) {
            log().error("Cannot find resource %s", filename);
            return nullptr;
        }
        SharedPtr<T> resource = makeShared<T>(context());
        resource_cache_.emplace(name, resource);
        log().info("Loading asset '%s'", filename);
        resource->load(filename, *resource_data.get());
        return resource;
    }

private:
    SharedPtr<InputStream> getResourceData(const Path& filename);

    Map<ResourcePath, UniquePtr<ResourceLocation>> resource_location_bindings_;
    HashMap<String, SharedPtr<Resource>> resource_cache_;
};
}  // namespace dw
