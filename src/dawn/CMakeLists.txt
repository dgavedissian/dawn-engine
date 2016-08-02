# Dawn Engine
# Written by David Avedissian (c) 2012-2016 (git@davedissian.com)

set(H_FILES
    core/Context.h
    core/EventSystem.h
    core/FixedMemoryPool.h
    core/Log.h
    core/Object.h
    core/Singleton.h
    core/StateManager.h
    core/StringUtils.h
    core/Timer.h
    engine/Config.h
    engine/ConfigNode.h
    engine/ConfigNode.inl
    engine/Engine.h
    input/Input.h
    math/Defs.h
    math/Noise.h
    math/Rect.h
    math/StringHash.h
    math/Vec2i.h
    math/Vec3i.h
    math/Vec4i.h
    physics/PhysicsWorld.h
    renderer/Renderer.h
    scene/Position.h
    scene/SceneManager.h
    script/LuaState.h
    Common.h
    DawnEngine.h
    Platform.h
    Prerequisites.h)

set(CPP_FILES
    core/Context.cpp
    core/EventSystem.cpp
    core/Log.cpp
    core/StateManager.cpp
    core/StringUtils.cpp
    core/Timer.cpp
    engine/Config.cpp
    engine/ConfigNode.cpp
    engine/Engine.cpp
    input/Input.cpp
    math/Noise.cpp
    math/Rect.cpp
    math/StringHash.cpp
    math/Vec2i.cpp
    math/Vec3i.cpp
    math/Vec4i.cpp
    physics/PhysicsWorld.cpp
    renderer/Renderer.cpp
    scene/Position.cpp
    scene/SceneManager.cpp
    script/LuaState.cpp)

# Project
add_library(Dawn SHARED ${CPP_FILES} ${H_FILES})
enable_cpp14(Dawn)
enable_maximum_warnings(Dawn)
add_dependencies(Dawn ${EXTERNAL_DEPS})
set_output_dir(Dawn ${CMAKE_BINARY_DIR}/lib)
set_target_properties(Dawn PROPERTIES COMPILE_FLAGS "-DDW_BUILD -DDW_DLL")

# Copy over shared library dependencies
set(DEPS_DIR ${CMAKE_BINARY_DIR}/dependencies/install/lib)
get_target_property(LIB_DIR Dawn LIBRARY_OUTPUT_DIRECTORY)
if(APPLE)
    set_target_properties(Dawn PROPERTIES FRAMEWORK TRUE)
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
