/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "core/platform/Defines.h"

// Disable "warning C4251: '...' needs to have dll-interface to be used by clients of class '...'"
#if defined(DW_MSVC)
#pragma warning(disable : 4251)
#endif

// Version Information
#define DW_VERSION_MAJOR 0
#define DW_VERSION_MINOR 1
#define DW_VERSION_PATCH 0
#define DW_VERSION_STR "0.1.0 (Pre-alpha 1)"

#define ERROR_FATAL(x)               \
    {                                \
        log().error(x);              \
        throw std::runtime_error(x); \
    }

#include <cassert>

#include "core/BaseTypes.h"
#include "core/Preprocessor.h"

#include "core/Context.h"
#include "core/Object.h"
#include "core/Module.h"
#include "core/Log.h"

#include <imgui.h>
