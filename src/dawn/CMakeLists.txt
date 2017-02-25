# Dawn Engine
# Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)

set(SOURCE_FILES
    core/Collections.h
    core/Context.cpp
    core/Context.h
    core/EventSystem.cpp
    core/EventSystem.h
    core/FixedMemoryPool.h
    core/Log.cpp
    core/Log.h
    core/Object.cpp
    core/Object.h
    core/Option.h
    core/Singleton.h
    core/StateManager.cpp
    core/StateManager.h
    core/StringUtils.cpp
    core/StringUtils.h
    core/Timer.cpp
    core/Timer.h
    engine/App.cpp
    engine/App.h
    engine/Engine.cpp
    engine/Engine.h
    input/Input.cpp
    input/Input.h
    io/File.cpp
    io/File.h
    io/FileSystem.cpp
    io/FileSystem.h
    io/InputStream.cpp
    io/InputStream.h
    io/OutputStream.cpp
    io/OutputStream.h
    io/Path.cpp
    io/Path.h
    math/Colour.h
    math/Defs.h
    math/Noise.cpp
    math/Noise.h
    math/Rect.cpp
    math/Rect.h
    math/StringHash.cpp
    math/StringHash.h
    math/Vec2i.cpp
    math/Vec2i.h
    math/Vec3i.cpp
    math/Vec3i.h
    math/Vec4i.cpp
    math/Vec4i.h
    physics/PhysicsWorld.cpp
    physics/PhysicsWorld.h
    renderer/Geometry.h
    renderer/IndexBuffer.cpp
    renderer/IndexBuffer.h
    renderer/ManualMesh.cpp
    renderer/ManualMesh.h
    renderer/Material.cpp
    renderer/Material.h
    renderer/Mesh.cpp
    renderer/Mesh.h
    renderer/MeshBuilder.cpp
    renderer/MeshBuilder.h
    renderer/Node.cpp
    renderer/Node.h
    renderer/Renderer.cpp
    renderer/Renderer.h
    renderer/ShaderProgram.cpp
    renderer/ShaderProgram.h
    renderer/Texture.cpp
    renderer/Texture.h
    renderer/TriangleBuffer.cpp
    renderer/TriangleBuffer.h
    renderer/VertexBuffer.cpp
    renderer/VertexBuffer.h
    resource/Resource.cpp
    resource/Resource.h
    resource/ResourceCache.cpp
    resource/ResourceCache.h
    scene/Position.cpp
    scene/Position.h
    scene/SceneManager.cpp
    scene/SceneManager.h
    script/LuaState.cpp
    script/LuaState.h
    Common.h
    DawnEngine.h
    Platform.h
    Prerequisites.h)

# Project
add_library(Dawn SHARED ${SOURCE_FILES})
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

# Random extras
mirror_physical_directories(${SOURCE_FILES})
set_target_properties(Dawn PROPERTIES DEBUG_POSTFIX _d)

# Add include dirs and libraries
target_include_directories(Dawn PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${INCLUDE_DIRS})
target_link_libraries(Dawn ${LIBS})
