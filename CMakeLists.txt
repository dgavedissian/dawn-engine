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

    # Add include directories
    get_target_property(DAWN_INCLUDE_DIRS Dawn INCLUDE_DIRECTORIES)
    target_include_directories(${GAME_NAME} PRIVATE
        ${DAWN_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR})

    # Add libraries
    target_link_libraries(${GAME_NAME} Dawn)
    set_output_dir(${GAME_NAME} ${CMAKE_SOURCE_DIR}/bin)
    set_target_properties(${GAME_NAME} PROPERTIES DEBUG_POSTFIX _d)

    # Currently, Dawn can only be linked dynamically
    #set_target_properties(${GAME_NAME} PROPERTIES COMPILE_FLAGS "-DDW_DLL")

    # LuaJIT on 64-bit OS X requires these linker flags for executables
    if(APPLE)
        set_target_properties(${GAME_NAME} PROPERTIES LINK_FLAGS "-pagezero_size 10000 -image_base 100000000")
    endif()

    # Copy over shared library dependencies
    get_target_property(LIB_DIR Dawn LIBRARY_OUTPUT_DIRECTORY)
    set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin)
    if(WIN32)
        add_custom_command(TARGET ${GAME_NAME} POST_BUILD
            COMMAND if exist ${LIB_DIR}/Dawn_d.dll ${CMAKE_COMMAND} -E copy ${LIB_DIR}/Dawn_d.dll ${BIN_DIR}
            COMMAND if exist ${LIB_DIR}/Dawn.dll ${CMAKE_COMMAND} -E copy ${LIB_DIR}/Dawn.dll ${BIN_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy ${LIB_DIR}/irrKlang.dll ${BIN_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/dependencies/install/bin/lua.dll ${BIN_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy ${LIB_DIR}/SDL2.dll ${BIN_DIR})
    elseif(APPLE)
        set(BUNDLE_DIR ${BIN_DIR}/${GAME_NAME}.app)
        set(FRAMEWORKS_DIR ${BUNDLE_DIR}/Contents/Frameworks)
        add_custom_command(TARGET ${GAME_NAME} POST_BUILD
            COMMAND cp -r ${LIB_DIR}/Dawn.framework ${FRAMEWORKS_DIR}/Dawn.framework)
        add_custom_command(TARGET ${GAME_NAME} POST_BUILD
            COMMAND cp ${LIB_DIR}/libirrklang.dylib ${FRAMEWORKS_DIR})
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

