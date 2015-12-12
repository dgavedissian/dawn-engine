# Dawn Engine
# Copyright (c) 2012-2015 David Avedissian (avedissian.david@gmail.com)

##############################################################
# External Dependencies
##############################################################

set(DAWN_DEPS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../dependencies" CACHE STRING
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
set(INCLUDE_DIRS ${INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/BtOgre
    ${CMAKE_CURRENT_SOURCE_DIR}/FastDelegate
    ${CMAKE_CURRENT_SOURCE_DIR}/ImGui
    ${CMAKE_CURRENT_SOURCE_DIR}/LuaBridge
    ${CMAKE_CURRENT_SOURCE_DIR}/YamlCpp/include)


##############################################################
# Packaged Dependencies
##############################################################
include_directories(${INCLUDE_DIRS})

# BtOgre
add_library(BtOgre BtOgre/BtOgre.cpp)
set_target_properties(BtOgre PROPERTIES FOLDER "3rdparty")
set(LIBS ${LIBS} BtOgre)

# ImGui
add_library(ImGui ImGui/imgui.cpp)
set_target_properties(ImGui PROPERTIES FOLDER "3rdparty")
set(LIBS ${LIBS} ImGui)

# YamlCpp
file(GLOB YAML_CPP_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} YamlCpp/src/*.cpp)
add_library(YamlCpp ${YAML_CPP_SRC})
set_target_properties(YamlCpp PROPERTIES FOLDER "3rdparty")
set(LIBS ${LIBS} YamlCpp)


##############################################################
# Push dependencies to the parent scope
##############################################################

set(INCLUDE_DIRS "${INCLUDE_DIRS}" PARENT_SCOPE)
set(LIBS "${LIBS}" PARENT_SCOPE)
