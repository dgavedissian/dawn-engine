# Dawn Engine
# Copyright (c) 2012-2015 David Avedissian (avedissian.david@gmail.com)
cmake_minimum_required(VERSION 3.0)
project(Dawn)


###############################################
# Setup
###############################################

# Include modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMake")


###############################################
# Platform specific stuff
###############################################

# Enable SSE4.1
if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
endif()

# Enable C++11
if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

    # If on OS X, use the libc++ standard library which is much more c++11 complete
    if(APPLE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
        if(XCODE)
            set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
            set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
        endif()
    endif()
endif()

# Enable lots of warnings
if(MSVC)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
elseif(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra -pedantic -Wuninitialized \
        -Woverloaded-virtual -Wno-deprecated-declarations -Wno-unused-parameter")
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-nested-anon-types -Wno-gnu-anonymous-struct")
    endif()
endif()

# Fix for "error C3859: virtual memory range for PCH exceeded" with MSVC
if(MSVC)
    string(REGEX REPLACE "/Zm[0-9]+ " "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm500" CACHE STRING "" FORCE)
endif()


###############################################
# Dependencies
###############################################

set(DAWN_DEPS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies" CACHE STRING
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
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/ThirdParty
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/ThirdParty/yaml-cpp/include)

# Push dependencies to the parent scope
set(INCLUDE_DIRS "${INCLUDE_DIRS}" PARENT_SCOPE)
set(LIBS "${LIBS}" PARENT_SCOPE)


###############################################
# Source files
###############################################

set(HEADER_FILES
    Source/Dawn/Audio/Sound.h
    Source/Dawn/Audio/SoundManager.h
    Source/Dawn/Audio/Source.h
    Source/Dawn/Audio/Track.h
    Source/Dawn/Core/Config.h
    Source/Dawn/Core/ConfigNode.h
    Source/Dawn/Core/ConfigNode.inl
    Source/Dawn/Core/FixedMemoryPool.h
    Source/Dawn/Core/Log.h
    Source/Dawn/Core/Singleton.h
    Source/Dawn/Core/Utils.h
    Source/Dawn/Graphics/Deferred/AmbientLight.h
    Source/Dawn/Graphics/Deferred/DeferredLight.h
    Source/Dawn/Graphics/Deferred/DeferredLightCompPass.h
    Source/Dawn/Graphics/Deferred/DeferredShadingManager.h
    Source/Dawn/Graphics/Deferred/GBufferMaterialGenerator.h
    Source/Dawn/Graphics/Deferred/GBufferSchemeHandler.h
    Source/Dawn/Graphics/Deferred/LightMaterialGenerator.h
    Source/Dawn/Graphics/Deferred/ListenerFactoryLogic.h
    Source/Dawn/Graphics/Deferred/MaterialGenerator.h
    Source/Dawn/Graphics/RenderSystem.h
    Source/Dawn/Graphics/SpriteManager.h
    Source/Dawn/Input/InputManager.h
    Source/Dawn/Math/Rect.h
    Source/Dawn/Math/Vec2i.h
    Source/Dawn/Math/Vec3i.h
    Source/Dawn/Math/Vec4i.h
    Source/Dawn/Physics/PhysicsManager.h
    Source/Dawn/Scene/Space/AsteroidBelt.h
    Source/Dawn/Scene/Space/Barycentre.h
    Source/Dawn/Scene/Space/Galaxy.h
    Source/Dawn/Scene/Space/Orbit.h
    Source/Dawn/Scene/Space/Planet.h
    Source/Dawn/Scene/Space/Star.h
    Source/Dawn/Scene/Space/StarSystem.h
    Source/Dawn/Scene/Space/SystemBody.h
    Source/Dawn/Scene/Camera.h
    Source/Dawn/Scene/DefaultCamera.h
    Source/Dawn/Scene/Entity.h
    Source/Dawn/Scene/Position.h
    Source/Dawn/Scene/RigidEntity.h
    Source/Dawn/Scene/SceneManager.h
    Source/Dawn/Script/LuaState.h
    Source/Dawn/UI/Console.h
    Source/Dawn/UI/ImGuiInterface.h
    Source/Dawn/UI/InterfaceManager.h
    Source/Dawn/UI/Layout.h
    Source/Dawn/UI/RocketInterface.h
    Source/Dawn/StateManager.h
    Source/Dawn/Common.h
    Source/Dawn/DawnEngine.h
    Source/Dawn/Engine.h
    Source/Dawn/EventSystem.h
    Source/Dawn/Platform.h
    Source/Dawn/Prerequisites.h)

set(SOURCE_FILES
    Source/Dawn/Audio/Sound.cpp
    Source/Dawn/Audio/SoundManager.cpp
    Source/Dawn/Audio/Source.cpp
    Source/Dawn/Audio/Track.cpp
    Source/Dawn/Core/Config.cpp
    Source/Dawn/Core/ConfigNode.cpp
    Source/Dawn/Core/Log.cpp
    Source/Dawn/Core/Utils.cpp
    Source/Dawn/Graphics/Deferred/AmbientLight.cpp
    Source/Dawn/Graphics/Deferred/DeferredLight.cpp
    Source/Dawn/Graphics/Deferred/DeferredLightCompPass.cpp
    Source/Dawn/Graphics/Deferred/DeferredShadingManager.cpp
    Source/Dawn/Graphics/Deferred/GBufferMaterialGenerator.cpp
    Source/Dawn/Graphics/Deferred/GBufferSchemeHandler.cpp
    Source/Dawn/Graphics/Deferred/LightMaterialGenerator.cpp
    Source/Dawn/Graphics/Deferred/MaterialGenerator.cpp
    Source/Dawn/Graphics/RenderSystem.cpp
    Source/Dawn/Graphics/SpriteManager.cpp
    Source/Dawn/Input/InputManager.cpp
    Source/Dawn/Math/Rect.cpp
    Source/Dawn/Math/Vec2i.cpp
    Source/Dawn/Math/Vec3i.cpp
    Source/Dawn/Math/Vec4i.cpp
    Source/Dawn/Physics/PhysicsManager.cpp
    Source/Dawn/Scene/Space/AsteroidBelt.cpp
    Source/Dawn/Scene/Space/Barycentre.cpp
    Source/Dawn/Scene/Space/Galaxy.cpp
    Source/Dawn/Scene/Space/Orbit.cpp
    Source/Dawn/Scene/Space/Planet.cpp
    Source/Dawn/Scene/Space/Star.cpp
    Source/Dawn/Scene/Space/StarSystem.cpp
    Source/Dawn/Scene/Space/SystemBody.cpp
    Source/Dawn/Scene/Camera.cpp
    Source/Dawn/Scene/DefaultCamera.cpp
    Source/Dawn/Scene/Entity.cpp
    Source/Dawn/Scene/Position.cpp
    Source/Dawn/Scene/RigidEntity.cpp
    Source/Dawn/Scene/SceneManager.cpp
    Source/Dawn/Script/LuaState.cpp
    Source/Dawn/UI/Console.cpp
    Source/Dawn/UI/ImGuiInterface.cpp
    Source/Dawn/UI/InterfaceManager.cpp
    Source/Dawn/UI/Layout.cpp
    Source/Dawn/UI/RocketInterface.cpp
    Source/Dawn/StateManager.cpp
    Source/Dawn/Engine.cpp
    Source/Dawn/EventSystem.cpp)

if(APPLE)
    set(HEADER_FILES ${HEADER_FILES} Source/Dawn/Graphics/OSXGetNSView.h)
    set(SOURCE_FILES ${SOURCE_FILES} Source/Dawn/Graphics/OSXGetNSView.mm)
endif()

# BtOgre
set(TP_SOURCE_FILES
    Source/ThirdParty/BtOgre.cpp
    Source/ThirdParty/imgui.cpp)

# yaml-cpp
file(GLOB YAML_CPP_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} Source/ThirdParty/yaml-cpp/src/*.cpp)
set(TP_SOURCE_FILES ${TP_SOURCE_FILES} ${YAML_CPP_SRC})


###############################################
# Project
###############################################

# Add the library
add_library(Dawn ${SOURCE_FILES} ${TP_SOURCE_FILES} ${HEADER_FILES})
set_target_properties(Dawn PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
set_target_properties(Dawn PROPERTIES COMPILE_FLAGS "-DDW_BUILD")
set_target_properties(Dawn PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "Source/Dawn/Prerequisites.h")

if(WIN32)
    set_target_properties(Dawn PROPERTIES DEBUG_POSTFIX _d)
    
    # Mirror the directory structure in visual studio
    set(ALL_FILES ${SOURCE_FILES} ${TP_SOURCE_FILES} ${HEADER_FILES})
    foreach(FILE ${ALL_FILES}) 
        get_filename_component(PARENT_DIR "${FILE}" PATH)

        # skip source dir and changes /'s to \\'s
        string(REGEX REPLACE "(\\./)?(Source)/?" "" GROUP "${PARENT_DIR}")
        string(REPLACE "/" "\\" GROUP "${GROUP}")
        source_group("${GROUP}" FILES "${FILE}")
    endforeach()
endif()

# Add include dirs and libraries
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Dawn
    ${INCLUDE_DIRS})
target_link_libraries(Dawn ${LIBS})

# Add the sandbox
add_executable(Sandbox Source/Sandbox/Main.cpp)
target_link_libraries(Sandbox Dawn)

# Run cotire
include(cotire)
cotire(Dawn)

###############################################
# Installation
###############################################
# TODO
