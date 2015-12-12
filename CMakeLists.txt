# Dawn Engine
# Copyright (c) 2012-2015 David Avedissian (avedissian.david@gmail.com)
cmake_minimum_required(VERSION 3.0)
project(Dawn)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)


###############################################
# Setup
###############################################

# Include modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

# Limit configuration types
set(CMAKE_CONFIGURATION_TYPES Release RelWithDebInfo Debug)


###############################################
# Utilities
###############################################

include(Utilities)
enable_cpp11()
enable_maximum_warnings()


###############################################
# Platform specific stuff
###############################################

# Fix for "error C3859: virtual memory range for PCH exceeded" with MSVC
if(MSVC)
    string(REGEX REPLACE "/Zm[0-9]+ " "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm500" CACHE STRING "" FORCE)
endif()


###############################################
# Helpful functions
###############################################

function(add_game GAME_NAME)
    # Executable type
    if(WIN32)
        set(TYPE WIN32)
    elseif(APPLE)
        set(TYPE MACOSX_BUNDLE)
    endif()

    # Necessary linker flags on 64-bit OS X for LuaJIT
    if(APPLE)
        set(CMAKE_EXE_LINKER_FLAGS "-pagezero_size 10000 -image_base 100000000")
    endif()

    # Add executable
    add_executable(${GAME_NAME} ${TYPE} ${ARGN})
endfunction()


###############################################
# Source
###############################################

add_subdirectory(src)


###############################################
# Installation
###############################################
# TODO
