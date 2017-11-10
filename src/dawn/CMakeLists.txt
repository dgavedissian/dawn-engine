# Dawn Engine
# Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)

set(SOURCE_FILES
    core/Collections.h
    core/Concurrency.cpp
    core/Concurrency.h
    core/Context.cpp
    core/Context.h
    core/Delegate.h
    core/EventData.h
    core/EventSystem.cpp
    core/EventSystem.h
    core/Exception.h
    core/FixedMemoryPool.h
	core/Handle.h
    core/Log.cpp
    core/Log.h
    core/Memory.cpp
    core/Memory.h
    core/Object.cpp
    core/Object.h
    core/Option.h
    core/Preprocessor.h
    core/StateManager.cpp
    core/StateManager.h
    core/StringUtils.cpp
    core/StringUtils.h
    core/Timer.cpp
    core/Timer.h
    ecs/Component.cpp
    ecs/Component.h
    ecs/Entity.cpp
    ecs/Entity.h
    ecs/EntityManager.cpp
    ecs/EntityManager.h
    ecs/System.h
    ecs/SystemManager.cpp
    ecs/SystemManager.h
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
    io/StringInputStream.cpp
    io/StringInputStream.h
    math/Colour.h
    math/Colour.cpp
    math/Defs.h
    math/MathGeoLib.h
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
    net/BitStream.cpp
    net/BitStream.h
    net/NetData.cpp
    net/NetData.h
    net/NetSystem.cpp
    net/NetSystem.h
    physics/BulletDynamics.h
    physics/PhysicsSystem.cpp
    physics/PhysicsSystem.h
    platform/Defines.h
    platform/Windows.h
    renderer/api/GL.h
    renderer/api/GLRenderContext.cpp
    renderer/api/GLRenderContext.h
    renderer/BillboardSet.cpp
    renderer/BillboardSet.h
    renderer/Camera.cpp
    renderer/Camera.h
    renderer/CustomMeshRenderable.cpp
    renderer/CustomMeshRenderable.h
    renderer/EntityRenderer.cpp
    renderer/EntityRenderer.h
    renderer/GLSL.h
    renderer/IndexBuffer.cpp
    renderer/IndexBuffer.h
    renderer/Material.cpp
    renderer/Material.h
    renderer/Mesh.cpp
    renderer/Mesh.h
    renderer/MeshBuilder.cpp
    renderer/MeshBuilder.h
    renderer/Program.cpp
    renderer/Program.h
    renderer/Renderable.h
    renderer/Renderable.cpp
    renderer/Renderer.cpp
    renderer/Renderer.h
    renderer/Shader.cpp
    renderer/Shader.h
    renderer/SPIRV.h
    renderer/StbImage.h
    renderer/Texture.cpp
    renderer/Texture.h
    renderer/TriangleBuffer.cpp
    renderer/TriangleBuffer.h
    renderer/VertexBuffer.cpp
    renderer/VertexBuffer.h
    renderer/VertexDecl.cpp
    renderer/VertexDecl.h
    resource/Resource.cpp
    resource/Resource.h
    resource/ResourceCache.cpp
    resource/ResourceCache.h
    scene/CameraController.cpp
    scene/CameraController.h
    scene/Position.cpp
    scene/Position.h
    scene/Transform.cpp
    scene/Transform.h
    scene/Universe.cpp
    scene/Universe.h
    script/LuaState.cpp
    script/LuaState.h
    script/Sol.h
    ui/Imgui.h
    ui/UserInterface.cpp
    ui/UserInterface.h
    Common.h
    DawnEngine.h
    Prerequisites.h)

# TODO: Replace prototype Ontology based ECS with custom implementation.
add_definitions(-DONTOLOGY_WITH_ASSERT)
set (SOURCE_FILES
    ${SOURCE_FILES}
    ecs/ontology/include/ontology/Config.hpp
    ecs/ontology/include/ontology/Component.hpp
    ecs/ontology/include/ontology/Entity.hpp
    ecs/ontology/include/ontology/Entity.hxx
    ecs/ontology/include/ontology/EntityManager.hpp
    ecs/ontology/include/ontology/EntityManagerInterface.hpp
    ecs/ontology/include/ontology/EntityManagerListener.hpp
    ecs/ontology/include/ontology/Exception.hpp
    ecs/ontology/include/ontology/ListenerDispatcher.hpp
    ecs/ontology/include/ontology/ListenerDispatcher.hxx
    ecs/ontology/include/ontology/System.hpp
    ecs/ontology/include/ontology/System.hxx
    ecs/ontology/include/ontology/SystemManager.hpp
    ecs/ontology/include/ontology/SystemManager.hxx
    ecs/ontology/include/ontology/Type.hpp
    ecs/ontology/include/ontology/Type.hxx
    ecs/ontology/include/ontology/TypeContainers.hpp
    ecs/ontology/include/ontology/World.hpp
    ecs/ontology/include/ontology/Ontology.hpp
    ecs/ontology/src/Component.cpp
    ecs/ontology/src/Entity.cpp
    ecs/ontology/src/EntityManager.cpp
    ecs/ontology/src/EntityManagerListener.cpp
    ecs/ontology/src/Exception.cpp
    ecs/ontology/src/System.cpp
    ecs/ontology/src/SystemManager.cpp
    ecs/ontology/src/Type.cpp
    ecs/ontology/src/World.cpp
)

# Project
set(BUILD_SHARED_LIBS OFF)
add_library(Dawn ${SOURCE_FILES})
enable_cpp14(Dawn)
enable_maximum_warnings(Dawn)
set_output_dir(Dawn ${CMAKE_BINARY_DIR}/lib)
if(BUILD_SHARED_LIBS)
    set_target_properties(Dawn PROPERTIES COMPILE_FLAGS "-DDW_BUILD -DDW_DLL")
endif()
target_include_directories(Dawn PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ecs/ontology/include)

# Copy over shared library dependencies
set(DEPS_DIR ${CMAKE_BINARY_DIR}/dependencies/install/lib)
get_target_property(LIB_DIR Dawn LIBRARY_OUTPUT_DIRECTORY)
if(APPLE AND BUILD_SHARED_LIBS)
    set_target_properties(Dawn PROPERTIES FRAMEWORK TRUE)
endif()

# Random extras
mirror_physical_directories(${SOURCE_FILES})
set_target_properties(Dawn PROPERTIES DEBUG_POSTFIX _d)

# Add include dirs and libraries
target_include_directories(Dawn PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${INCLUDE_DIRS})
target_link_libraries(Dawn ${LIBS})
