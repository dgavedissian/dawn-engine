# Dawn Engine
# Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
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

    # Add executable
    add_executable(${GAME_NAME} ${TYPE} ${ARGN})
    target_link_libraries(${GAME_NAME} Dawn)
    if(APPLE)
        target_link_libraries(${GAME_NAME} -pagezero_size 10000 -image_base 100000000)
    endif()
    set_target_properties(${GAME_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

    # Copy over shared library dependencies
    get_target_property(LIB_DIR Dawn LIBRARY_OUTPUT_DIRECTORY)
    set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin)
    if(WIN32)
        # TODO: Windows
    elseif(APPLE)
        set(BUNDLE_DIR ${BIN_DIR}/${GAME_NAME}.app)
        set(FRAMEWORKS_DIR ${BUNDLE_DIR}/Contents/Frameworks)
        add_custom_command(TARGET ${GAME_NAME} POST_BUILD
            COMMAND cp -r ${LIB_DIR}/Dawn.framework ${FRAMEWORKS_DIR})
        add_custom_command(TARGET ${GAME_NAME} POST_BUILD
            COMMAND cp ${LIB_DIR}/libirrklang.dylib ${BUNDLE_DIR}/Contents/MacOS)
    else()
        # TODO: Linux
    endif()
endfunction()


###############################################
# Source
###############################################

add_subdirectory(src)


###############################################
# Installation
###############################################
# TODO
