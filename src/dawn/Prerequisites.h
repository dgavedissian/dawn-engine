/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "core/platform/Defines.h"

// Mark this header as a system header
#if defined(DW_GCC) || defined(DW_CLANG)
#pragma GCC system_header
#elif defined(DW_MSVC)
#pragma warning(push, 0)
#endif

//
// Standard library
//

#include <cassert>
#include <cstdint>
#include <ctime>
#include <climits>

#include <exception>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <random>
#include <memory>
#include <functional>
#include <limits>
#include <typeinfo>
#include <chrono>
#include <type_traits>

//
// External Libraries
//

//
// Bundled Libraries
//

// imgui
//#include "imgui.h"

// Re-enable warnings
#if defined(DW_MSVC)
#pragma warning(pop)
#endif

// Disable "warning C4251: '...' needs to have dll-interface to be used by clients of class '...'"
#if defined(DW_MSVC)
#pragma warning(disable : 4251)
#endif
