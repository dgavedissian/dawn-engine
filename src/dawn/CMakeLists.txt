# Dawn Engine
# Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)

set(SOURCE_FILES
    core/io/File.cpp
    core/io/File.h
    core/io/FileSystem.cpp
    core/io/FileSystem.h
    core/io/InputStream.cpp
    core/io/InputStream.h
    core/io/OutputStream.cpp
    core/io/OutputStream.h
    core/io/Path.cpp
    core/io/Path.h
    core/io/StringInputStream.cpp
    core/io/StringInputStream.h
    core/math/Colour.h
    core/math/Colour.cpp
    core/math/Defs.h
    core/math/MathGeoLib.h
    core/math/Noise.cpp
    core/math/Noise.h
    core/math/Rect.cpp
    core/math/Rect.h
    core/math/StringHash.h
    core/math/Vec2i.cpp
    core/math/Vec2i.h
    core/math/Vec3i.cpp
    core/math/Vec3i.h
    core/math/Vec4i.cpp
    core/math/Vec4i.h
    core/platform/Defines.h
    core/platform/Windows.h
    core/script/LuaState.cpp
    core/script/LuaState.h
    core/script/Sol.h
    core/App.cpp
    core/App.h
    core/Collections.h
    core/Concurrency.cpp
    core/Concurrency.h
    core/Context.cpp
    core/Context.h
    core/Delegate.h
    core/Engine.cpp
    core/Engine.h
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
    core/Module.cpp
    core/Module.h
    core/Object.cpp
    core/Object.h
    core/Option.h
    core/Position.cpp
    core/Position.h
    core/Preprocessor.h
    core/StringUtils.cpp
    core/StringUtils.h
    core/Timer.cpp
    core/Timer.h
    gameplay/GameMode.cpp
    gameplay/GameMode.h
    gameplay/GameplayModule.cpp
    gameplay/GameplayModule.h
    input/Input.cpp
    input/Input.h
    net/BitStream.cpp
    net/BitStream.h
    net/NetData.cpp
    net/NetData.h
    net/NetEntityPipeline.cpp
    net/NetEntityPipeline.h
    net/NetGameMode.cpp
    net/NetGameMode.h
    net/NetRole.h
    net/NetTransform.cpp
    net/NetTransform.h
    net/Networking.cpp
    net/Networking.h
    net/RepProperty.h
    net/RepProperty.i.h
    net/Rpc.cpp
    net/Rpc.h
    net/Rpc.i.h
    net/Yojimbo.h
    renderer/rhi/GL.h
    renderer/rhi/GLRenderContext.cpp
    renderer/rhi/GLRenderContext.h
    renderer/rhi/NullRenderContext.cpp
    renderer/rhi/NullRenderContext.h
    renderer/rhi/Renderer.cpp
    renderer/rhi/Renderer.h
    renderer/rhi/SPIRV.h
    renderer/rhi/VertexDecl.cpp
    renderer/rhi/VertexDecl.h
    renderer/BillboardSet.cpp
    renderer/BillboardSet.h
    renderer/Camera.cpp
    renderer/Camera.h
    renderer/CustomMeshRenderable.cpp
    renderer/CustomMeshRenderable.h
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
    renderer/SceneNode.h
    renderer/SceneNode.cpp
    renderer/Shader.cpp
    renderer/Shader.h
    renderer/StbImage.h
    renderer/Texture.cpp
    renderer/Texture.h
    renderer/Transform.cpp
    renderer/Transform.h
    renderer/TriangleBuffer.cpp
    renderer/TriangleBuffer.h
    renderer/VertexBuffer.cpp
    renderer/VertexBuffer.h
    resource/Resource.cpp
    resource/Resource.h
    resource/ResourceCache.cpp
    resource/ResourceCache.h
    scene/BulletDynamics.h
    scene/CameraController.cpp
    scene/CameraController.h
    scene/Component.cpp
    scene/Component.h
    scene/Entity.cpp
    scene/Entity.h
    scene/PhysicsScene.cpp
    scene/PhysicsScene.h
    scene/SceneManager.cpp
    scene/SceneManager.h
    scene/System.cpp
    scene/System.h
        scene/TransformComponent.cpp
        scene/TransformComponent.h
    scene/Velocity.h
    scene/VelocitySystem.cpp
    scene/VelocitySystem.h
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
        scene/ontology/include/ontology/Config.hpp
        scene/ontology/include/ontology/Component.hpp
        scene/ontology/include/ontology/Entity.hpp
        scene/ontology/include/ontology/Entity.hxx
        scene/ontology/include/ontology/EntityManager.hpp
        scene/ontology/include/ontology/EntityManagerInterface.hpp
        scene/ontology/include/ontology/EntityManagerListener.hpp
        scene/ontology/include/ontology/Exception.hpp
        scene/ontology/include/ontology/ListenerDispatcher.hpp
        scene/ontology/include/ontology/ListenerDispatcher.hxx
        scene/ontology/include/ontology/System.hpp
        scene/ontology/include/ontology/System.hxx
        scene/ontology/include/ontology/SystemManager.hpp
        scene/ontology/include/ontology/SystemManager.hxx
        scene/ontology/include/ontology/Type.hpp
        scene/ontology/include/ontology/Type.hxx
        scene/ontology/include/ontology/TypeContainers.hpp
        scene/ontology/include/ontology/World.hpp
        scene/ontology/include/ontology/Ontology.hpp
        scene/ontology/src/Component.cpp
        scene/ontology/src/Entity.cpp
        scene/ontology/src/EntityManager.cpp
        scene/ontology/src/EntityManagerListener.cpp
        scene/ontology/src/Exception.cpp
        scene/ontology/src/System.cpp
        scene/ontology/src/SystemManager.cpp
        scene/ontology/src/Type.cpp
        scene/ontology/src/World.cpp)

# Project
set(BUILD_SHARED_LIBS OFF)
add_library(DwEngine ${SOURCE_FILES})
enable_cpp14(DwEngine)
enable_maximum_warnings(DwEngine)
set_output_dir(DwEngine ${CMAKE_BINARY_DIR}/lib)
if(BUILD_SHARED_LIBS)
    set_target_properties(DwEngine PROPERTIES COMPILE_FLAGS "-DDW_BUILD -DDW_DLL")
endif()
target_include_directories(DwEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
        scene/ontology/include)

# Copy over shared library dependencies
set(DEPS_DIR ${CMAKE_BINARY_DIR}/dependencies/install/lib)
get_target_property(LIB_DIR DwEngine LIBRARY_OUTPUT_DIRECTORY)
if(APPLE AND BUILD_SHARED_LIBS)
    set_target_properties(DwEngine PROPERTIES FRAMEWORK TRUE)
endif()

# Random extras
mirror_physical_directories(${SOURCE_FILES})
set_target_properties(DwEngine PROPERTIES DEBUG_POSTFIX _d)

# Add include dirs and libraries
target_include_directories(DwEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${INCLUDE_DIRS})
target_link_libraries(DwEngine ${LIBS})
