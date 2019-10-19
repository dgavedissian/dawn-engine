/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "Path.h"
#include "core/StringUtils.h"

namespace dw {
Path simplifyAbsolutePath(const Path& path) {
    if (path[0] == '/') {
        Vector<String> path_components;
        Vector<String> new_path_components;
        str::split(path, '/', path_components);
        for (auto& component : path_components) {
            if (component == ".") {
                continue;
            }
            if (component == "..") {
                new_path_components.pop_back();
                continue;
            }
            new_path_components.push_back(component);
        }
        return str::join(new_path_components, "/");
    }
    return path;
}
}  // namespace dw
