# Dawn Engine
# Copyright (c) 2012-2015 David Avedissian (avedissian.david@gmail.com)
cmake_minimum_required(VERSION 3.0)
project(Dawn)


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
# Dependencies
###############################################

set(DAWN_DEPS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dependencies" CACHE STRING
    "Directory to an extracted Dawn Engine dependencies package")

# Locations of dependencies
set(BULLET_ROOT "${DAWN_DEPS_DIR}")
set(IRRKLANG_DIR "${DAWN_DEPS_DIR}")
set(LUA_DIR "${DAWN_DEPS_DIR}")
set(MATHGEOLIB_DIR "${DAWN_DEPS_DIR}")
set(OGRE_HOME "${DAWN_DEPS_DIR}")
set(PARTICLEUNIVERSE_DIR "${DAWN_DEPS_DIR}")
set(ROCKETDIR "${DAWN_DEPS_DIR}")
set(SDL2DIR "${DAWN_DEPS_DIR}")

# Dependencies
find_package(Bullet REQUIRED)                           # physics
find_package(IrrKlang REQUIRED)                         # audio
find_package(Lua REQUIRED)                              # scripting
find_package(MathGeoLib REQUIRED)                       # vectors and matrices
find_package(OGRE REQUIRED)                             # rendering
find_package(ParticleUniverse REQUIRED)                 # particles
find_package(Rocket REQUIRED Core Controls Debugger)    # gui
find_package(SDL2 REQUIRED)                             # windowing and input

set(INCLUDE_DIRS
    ${BULLET_INCLUDE_DIRS}
    ${IRRKLANG_INCLUDE_DIR}
    ${LUA_INCLUDE_DIR}
    ${MATHGEOLIB_INCLUDE_DIR}
    ${OGRE_INCLUDE_DIRS}
    ${PARTICLEUNIVERSE_INCLUDE_DIR}
    ${ROCKET_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIR})

set(LIBS
    ${BULLET_LIBRARIES}
    ${IRRKLANG_LIBRARIES}
    ${LUA_LIBRARIES}
    ${MATHGEOLIB_LIBRARY}
    ${OGRE_LIBRARIES}
    ${PARTICLEUNIVERSE_LIBRARIES}
    ${ROCKET_LIBRARIES}
    ${SDL2_LIBRARIES})

# Add bundled external libraries
set(INCLUDE_DIRS
    ${INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/3rdparty/BtOgre
    ${CMAKE_CURRENT_SOURCE_DIR}/src/3rdparty/FastDelegate
    ${CMAKE_CURRENT_SOURCE_DIR}/src/3rdparty/ImGui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/3rdparty/LuaBridge
    ${CMAKE_CURRENT_SOURCE_DIR}/src/3rdparty/YamlCpp/include)

# Push dependencies to the parent scope
if(NOT ${CMAKE_SOURCE_DIR} EQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    set(INCLUDE_DIRS "${INCLUDE_DIRS}" PARENT_SCOPE)
    set(LIBS "${LIBS}" PARENT_SCOPE)
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
