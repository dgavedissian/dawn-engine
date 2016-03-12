# Dawn Engine
# Written by David Avedissian (c) 2012-2016 (git@davedissian.com)

set(H_FILES
    audio/Audio.h
    audio/Sound.h
    audio/Source.h
    audio/Track.h
    core/Config.h
    core/ConfigNode.h
    core/ConfigNode.inl
    core/FixedMemoryPool.h
    core/Log.h
    core/Singleton.h
    core/Utils.h
    input/Input.h
    math/Defs.h
    math/Noise.h
    math/Rect.h
    math/Vec2i.h
    math/Vec3i.h
    math/Vec4i.h
    physics/PhysicsWorld.h
    renderer/deferred/AmbientLight.h
    renderer/deferred/DeferredLight.h
    renderer/deferred/DeferredLightCompPass.h
    renderer/deferred/DeferredShadingManager.h
    renderer/deferred/GBufferMaterialGenerator.h
    renderer/deferred/GBufferSchemeHandler.h
    renderer/deferred/LightMaterialGenerator.h
    renderer/deferred/ListenerFactoryLogic.h
    renderer/deferred/MaterialGenerator.h
    renderer/GeometryUtils.h
    renderer/Renderer.h
    renderer/SpriteManager.h
    scene/space/AsteroidBelt.h
    scene/space/Barycentre.h
    scene/space/Galaxy.h
    scene/space/Orbit.h
    scene/space/Planet.h
    scene/space/Star.h
    scene/space/StarSystem.h
    scene/space/SystemBody.h
    scene/Camera.h
    scene/DefaultCamera.h
    scene/Entity.h
    scene/Position.h
    scene/RigidEntity.h
    scene/SceneManager.h
    script/LuaState.h
    ui/Console.h
    ui/ImGuiInterface.h
    ui/Layout.h
    ui/RocketInterface.h
    ui/UI.h
    StateManager.h
    Common.h
    DawnEngine.h
    Engine.h
    EventSystem.h
    Platform.h
    Prerequisites.h)

set(CPP_FILES
    audio/Audio.cpp
    audio/Sound.cpp
    audio/Source.cpp
    audio/Track.cpp
    core/Config.cpp
    core/ConfigNode.cpp
    core/Log.cpp
    core/Utils.cpp
    input/Input.cpp
    math/Noise.cpp
    math/Rect.cpp
    math/Vec2i.cpp
    math/Vec3i.cpp
    math/Vec4i.cpp
    physics/PhysicsWorld.cpp
    renderer/deferred/AmbientLight.cpp
    renderer/deferred/DeferredLight.cpp
    renderer/deferred/DeferredLightCompPass.cpp
    renderer/deferred/DeferredShadingManager.cpp
    renderer/deferred/GBufferMaterialGenerator.cpp
    renderer/deferred/GBufferSchemeHandler.cpp
    renderer/deferred/LightMaterialGenerator.cpp
    renderer/deferred/MaterialGenerator.cpp
    renderer/GeometryUtils.cpp
    renderer/Renderer.cpp
    renderer/SpriteManager.cpp
    scene/space/AsteroidBelt.cpp
    scene/space/Barycentre.cpp
    scene/space/Galaxy.cpp
    scene/space/Orbit.cpp
    scene/space/Planet.cpp
    scene/space/Star.cpp
    scene/space/StarSystem.cpp
    scene/space/SystemBody.cpp
    scene/Camera.cpp
    scene/DefaultCamera.cpp
    scene/Entity.cpp
    scene/Position.cpp
    scene/RigidEntity.cpp
    scene/SceneManager.cpp
    script/LuaState.cpp
    ui/Console.cpp
    ui/ImGuiInterface.cpp
    ui/Layout.cpp
    ui/RocketInterface.cpp
    ui/UI.cpp
    StateManager.cpp
    Engine.cpp
    EventSystem.cpp)

if(APPLE)
    set(H_FILES ${H_FILES} renderer/OSXGetNSView.h)
    set(CPP_FILES ${CPP_FILES} renderer/OSXGetNSView.mm)
endif()

# Project
add_library(Dawn SHARED ${CPP_FILES} ${H_FILES})
add_dependencies(Dawn ${EXTERNAL_DEPS})
set_output_dir(Dawn ${CMAKE_BINARY_DIR}/lib)
set_target_properties(Dawn PROPERTIES COMPILE_FLAGS "-DDW_BUILD -DDW_DLL")

# Copy over shared library dependencies
set(DEPS_DIR ${CMAKE_BINARY_DIR}/dependencies/install/lib)
get_target_property(LIB_DIR Dawn LIBRARY_OUTPUT_DIRECTORY)
if(WIN32)
    add_custom_command(TARGET Dawn POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${DEPS_DIR}/irrKlang.dll ${LIB_DIR})
    # TODO: Use SDL2_LIBRARIES
    add_custom_command(TARGET Dawn POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_INCLUDE_DIR}/../lib/x86/SDL2.dll ${LIB_DIR})
elseif(APPLE)
    set_target_properties(Dawn PROPERTIES FRAMEWORK TRUE)
    add_custom_command(TARGET Dawn POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${DEPS_DIR}/libirrklang.dylib ${LIB_DIR})
endif()

# Cotire
if(WIN32)
    include(cotire)
    set_target_properties(Dawn PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
    set_target_properties(Dawn PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "Prerequisites.h")
    cotire(Dawn)
endif()

# Random extras
mirror_physical_directories(${CPP_FILES} ${H_FILES})
set_target_properties(Dawn PROPERTIES DEBUG_POSTFIX _d)

# Add include dirs and libraries
target_include_directories(Dawn PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${INCLUDE_DIRS})
target_link_libraries(Dawn ${LIBS})
