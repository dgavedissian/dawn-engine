/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "core/Result.h"
#include "core/io/File.h"
#include "resource/Resource.h"

namespace dw {
using ResourcePath = String;

Pair<String, Path> parseResourcePath(const ResourcePath& resource_path);

class DW_API ResourceLocation {
public:
    virtual ~ResourceLocation() = default;
    virtual Result<SharedPtr<InputStream>, String> getFile(
        const ResourcePath& path_within_location) = 0;
};

class DW_API ResourcePackage : public Object, public ResourceLocation {
public:
    DW_OBJECT(ResourcePackage);

    ResourcePackage(Context* ctx, const Path& package);

    Result<SharedPtr<InputStream>, String> getFile(
        const ResourcePath& path_within_location) override;
};

class DW_API ResourceFilesystemPath : public Object, public ResourceLocation {
public:
    DW_OBJECT(ResourceFilesystemPath);

    ResourceFilesystemPath(Context* ctx, const Path& path);

    Result<SharedPtr<InputStream>, String> getFile(
        const ResourcePath& path_within_location) override;

private:
    Path path_;
};

class DW_API ResourceCache : public Module {
public:
    DW_OBJECT(ResourceCache);

    ResourceCache(Context* context);
    ~ResourceCache();

    void addPath(const String& package, const Path& path);
    void addPackage(const String& package, UniquePtr<ResourcePackage> file);

    template <typename T>
    Result<SharedPtr<T>, String> addCustomResource(const ResourcePath& resource_path, SharedPtr<T> resource) {
        String name(resource_path);

        if (!resource) {
            return {str::format("NULL resource provided at %s. Skipping.", name)};
        }

        if (resource_cache_.find(name) != resource_cache_.end()) {
            log().warn("Found an existing resource at %s. Replacing with an instance of '%s'", name,
                       resource->typeName());
        }
        resource_cache_[name] = resource;
        return {resource};
    }

    template <typename T> Result<SharedPtr<T>, String> get(const ResourcePath& resource_path) {
        String name(resource_path);

        // If the resource already exists, return it.
        auto it = resource_cache_.find(name);
        if (it != resource_cache_.end()) {
            return staticPointerCast<T>((*it).second);
        }

        // Load the file which contains this resource data.
        auto resource_data = getResourceData(resource_path);
        if (resource_data.hasError()) {
            return {str::format("Cannot find resource %s. Reason: %s", resource_path, resource_data.getError())};
        }
        SharedPtr<T> resource = makeShared<T>(context());
        resource_cache_.emplace(name, resource);
        log().info("Loading asset '%s'", resource_path);
        auto load_result = resource->load(resource_path, *resource_data.getValue().get());
        if (load_result.hasError())
        {
            return {str::format("Failed to load resource %s. Reason: %s", resource_path, load_result.getError())};
        }
        return resource;
    }

private:
    Result<SharedPtr<InputStream>, String> getResourceData(const Path& filename);

    Map<String, UniquePtr<ResourceLocation>> resource_packages_;
    HashMap<String, SharedPtr<Resource>> resource_cache_;
};
}  // namespace dw
