# Dawn Engine
# Copyright (c) 2012-2015 David Avedissian (avedissian.david@gmail.com)

set(H_FILES
    Audio/Audio.h
    Audio/Sound.h
    Audio/Source.h
    Audio/Track.h
    Core/Config.h
    Core/ConfigNode.h
    Core/ConfigNode.inl
    Core/FixedMemoryPool.h
    Core/Log.h
    Core/Singleton.h
    Core/Utils.h
    Input/Input.h
    Math/MathDefs.h
    Math/Noise.h
    Math/Rect.h
    Math/Vec2i.h
    Math/Vec3i.h
    Math/Vec4i.h
    Physics/PhysicsWorld.h
    Renderer/Deferred/AmbientLight.h
    Renderer/Deferred/DeferredLight.h
    Renderer/Deferred/DeferredLightCompPass.h
    Renderer/Deferred/DeferredShadingManager.h
    Renderer/Deferred/GBufferMaterialGenerator.h
    Renderer/Deferred/GBufferSchemeHandler.h
    Renderer/Deferred/LightMaterialGenerator.h
    Renderer/Deferred/ListenerFactoryLogic.h
    Renderer/Deferred/MaterialGenerator.h
    Renderer/GeometryUtils.h
    Renderer/Renderer.h
    Renderer/SpriteManager.h
    Scene/Space/AsteroidBelt.h
    Scene/Space/Barycentre.h
    Scene/Space/Galaxy.h
    Scene/Space/Orbit.h
    Scene/Space/Planet.h
    Scene/Space/Star.h
    Scene/Space/StarSystem.h
    Scene/Space/SystemBody.h
    Scene/Camera.h
    Scene/DefaultCamera.h
    Scene/Entity.h
    Scene/Position.h
    Scene/RigidEntity.h
    Scene/SceneManager.h
    Script/LuaState.h
    UI/Console.h
    UI/ImGuiInterface.h
    UI/Layout.h
    UI/RocketInterface.h
    UI/UI.h
    StateManager.h
    Common.h
    DawnEngine.h
    Engine.h
    EventSystem.h
    Platform.h
    Prerequisites.h)

set(CPP_FILES
    Audio/Audio.cpp
    Audio/Sound.cpp
    Audio/Source.cpp
    Audio/Track.cpp
    Core/Config.cpp
    Core/ConfigNode.cpp
    Core/Log.cpp
    Core/Utils.cpp
    Input/Input.cpp
    Math/Noise.cpp
    Math/Rect.cpp
    Math/Vec2i.cpp
    Math/Vec3i.cpp
    Math/Vec4i.cpp
    Physics/PhysicsWorld.cpp
    Renderer/Deferred/AmbientLight.cpp
    Renderer/Deferred/DeferredLight.cpp
    Renderer/Deferred/DeferredLightCompPass.cpp
    Renderer/Deferred/DeferredShadingManager.cpp
    Renderer/Deferred/GBufferMaterialGenerator.cpp
    Renderer/Deferred/GBufferSchemeHandler.cpp
    Renderer/Deferred/LightMaterialGenerator.cpp
    Renderer/Deferred/MaterialGenerator.cpp
    Renderer/GeometryUtils.cpp
    Renderer/Renderer.cpp
    Renderer/SpriteManager.cpp
    Scene/Space/AsteroidBelt.cpp
    Scene/Space/Barycentre.cpp
    Scene/Space/Galaxy.cpp
    Scene/Space/Orbit.cpp
    Scene/Space/Planet.cpp
    Scene/Space/Star.cpp
    Scene/Space/StarSystem.cpp
    Scene/Space/SystemBody.cpp
    Scene/Camera.cpp
    Scene/DefaultCamera.cpp
    Scene/Entity.cpp
    Scene/Position.cpp
    Scene/RigidEntity.cpp
    Scene/SceneManager.cpp
    Script/LuaState.cpp
    UI/Console.cpp
    UI/ImGuiInterface.cpp
    UI/Layout.cpp
    UI/RocketInterface.cpp
    UI/UI.cpp
    StateManager.cpp
    Engine.cpp
    EventSystem.cpp)

if(APPLE)
    set(H_FILES ${H_FILES} Graphics/OSXGetNSView.h)
    set(CPP_FILES} ${CPP_FILES} Graphics/OSXGetNSView.mm)
endif()

# BtOgre
set(TP_SOURCE_FILES
    ../ThirdParty/BtOgre/BtOgre.cpp)

# ImGui
set(TP_SOURCE_FILES ${TP_SOURCE_FILES}
    ../ThirdParty/ImGui/imgui.cpp)

# YamlCpp
file(GLOB YAML_CPP_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ../ThirdParty/YamlCpp/src/*.cpp)
set(TP_SOURCE_FILES ${TP_SOURCE_FILES} ${YAML_CPP_SRC})

# Project
set(SOURCE_FILES ${TP_SOURCE_FILES} ${CPP_FILES} ${H_FILES})
add_library(Dawn ${SOURCE_FILES})
set_target_properties(Dawn PROPERTIES COMPILE_FLAGS "-DDW_BUILD")

# Cotire
message(STATUS ${CMAKE_CXX_FLAGS} ${INCLUDE_DIRS})
set_target_properties(Dawn PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
set_target_properties(Dawn PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "Prerequisites.h")
include(cotire)
if(WIN32)
    cotire(Dawn)
endif()

# Random extras
mirror_physical_directories(${SOURCE_FILES})
if(WIN32)
    set_target_properties(Dawn PROPERTIES DEBUG_POSTFIX _d)
endif()

# Add include dirs and libraries
include_directories(
    .
    ${INCLUDE_DIRS})
target_link_libraries(Dawn ${LIBS})
