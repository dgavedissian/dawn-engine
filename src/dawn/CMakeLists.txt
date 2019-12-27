# Dawn Engine
# Written by David Avedissian (c) 2012-2019 (git@dga.dev)

message(STATUS "Setting up external dependencies...")
add_subdirectory(external)

flatbuffers_generate_cpp_headers(
    NetProtocol
    net/protocol/protocol.fbs
    net/protocol/to_client.fbs
    net/protocol/to_server.fbs
)

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
    core/App.h
    core/Collections.h
    core/CommandLine.cpp
    core/CommandLine.h
    core/Concurrency.h
    core/Context.cpp
    core/Context.h
    core/Delegate.h
    core/Engine.cpp
    core/Engine.h
    core/EventData.h
    core/EventSystem.cpp
    core/EventSystem.h
    core/FixedMemoryPool.h
    core/GameMode.cpp
    core/GameMode.h
    core/GameSession.cpp
    core/GameSession.h
    core/Log.cpp
    core/Log.h
    core/Module.cpp
    core/Module.h
    core/Object.cpp
    core/Object.h
    core/Preprocessor.h
    core/StringUtils.cpp
    core/StringUtils.h
    core/Timer.cpp
    core/Timer.h
    core/Type.h
    input/Input.cpp
    input/Input.h
    net/transport/InProcessTransport.cpp
    net/transport/InProcessTransport.h
    net/transport/ReliableUDPTransport.cpp
    net/transport/ReliableUDPTransport.h
    net/transport/Transport.h
    net/transport/Yojimbo.h
    net/BitStream.cpp
    net/BitStream.h
    net/CNetData.cpp
    net/CNetData.h
    net/CNetTransform.cpp
    net/CNetTransform.h
    net/NetEntityPipeline.cpp
    net/NetEntityPipeline.h
    net/NetGameMode.cpp
    net/NetGameMode.h
    net/NetInstance.cpp
    net/NetInstance.h
    net/NetMode.h
    net/NetRole.h
    net/RepProperty.h
    net/RepProperty.i.h
    net/Rpc.cpp
    net/Rpc.h
    net/Rpc.i.h
    renderer/BillboardSet.cpp
    renderer/BillboardSet.h
    renderer/CCamera.cpp
    renderer/CCamera.h
    renderer/CustomRenderable.cpp
    renderer/CustomRenderable.h
    renderer/FrameBuffer.cpp
    renderer/FrameBuffer.h
    renderer/IndexBuffer.cpp
    renderer/IndexBuffer.h
    renderer/Material.cpp
    renderer/Material.h
    renderer/Mesh.cpp
    renderer/Mesh.h
    renderer/Node.cpp
    renderer/Node.h
    renderer/Program.cpp
    renderer/Program.h
    renderer/Renderable.h
    renderer/Renderable.cpp
    renderer/Renderer.cpp
    renderer/Renderer.h
    renderer/RenderPipeline.cpp
    renderer/RenderPipeline.h
    renderer/RenderQueue.h
    renderer/SceneGraph.cpp
    renderer/SceneGraph.h
    renderer/Shader.cpp
    renderer/Shader.h
    renderer/StbImage.h
    renderer/SystemPosition.cpp
    renderer/SystemPosition.h
    renderer/Texture.cpp
    renderer/Texture.h
    renderer/VertexBuffer.cpp
    renderer/VertexBuffer.h
    resource/Resource.cpp
    resource/Resource.h
    resource/ResourceCache.cpp
    resource/ResourceCache.h
    scene/space/Barycentre.h
    scene/space/Orbit.cpp
    scene/space/Orbit.h
    scene/space/Planet.cpp
    scene/space/Planet.h
    scene/space/PlanetLod.cpp
    scene/space/PlanetLod.h
    scene/space/PlanetRings.cpp
    scene/space/PlanetRings.h
    scene/space/Star.cpp
    scene/space/Star.h
    scene/space/StarSystem.cpp
    scene/space/StarSystem.h
    scene/space/SystemBody.cpp
    scene/space/SystemBody.h
    scene/BulletDynamics.h
    scene/CLinearMotion.h
    scene/CameraController.cpp
    scene/CameraController.h
    scene/Component.cpp
    scene/Component.h
    scene/CSceneNode.cpp
    scene/CSceneNode.h
    scene/Entity.cpp
    scene/Entity.h
    scene/PhysicsScene.cpp
    scene/PhysicsScene.h
    scene/SLinearMotion.cpp
    scene/SLinearMotion.h
    scene/SceneManager.cpp
    scene/SceneManager.h
    script/LuaState.cpp
    script/LuaState.h
    script/Sol.h
    ui/Imgui.h
    ui/UserInterface.cpp
    ui/UserInterface.h
    Base.h
    Input.h
    Net.h
    Renderer.h
    Resource.h
    Scene.h
    UI.h)

# Project
set(BUILD_SHARED_LIBS OFF)
add_library(DwEngine ${SOURCE_FILES} ${FLATBUFFER_DEFINITION_FILES} ${NetProtocol_OUTPUTS})
target_compile_features(DwEngine PUBLIC cxx_std_17)
enable_maximum_warnings(DwEngine)
if(BUILD_SHARED_LIBS)
    target_compile_definitions(DwEngine PRIVATE DW_BUILD DW_DLL)
endif()
if(APPLE)
    target_compile_definitions(DwEngine PRIVATE _LIBCPP_DISABLE_AVAILABILITY)
endif()
target_include_directories(DwEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(DwEngine PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers)

# Copy over shared library dependencies
set(DEPS_DIR ${CMAKE_BINARY_DIR}/dependencies/install/lib)
get_target_property(LIB_DIR DwEngine LIBRARY_OUTPUT_DIRECTORY)
if(APPLE AND BUILD_SHARED_LIBS)
    set_target_properties(DwEngine PROPERTIES FRAMEWORK TRUE)
endif()

# Random extras
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCE_FILES} ${FLATBUFFER_DEFINITION_FILES})
set_target_properties(DwEngine PROPERTIES DEBUG_POSTFIX _d)

# Add dependencies.
target_link_libraries(DwEngine
    assimp
    BulletDynamics
    BulletCollision
    LinearMath
    concurrentqueue
    dawn-gfx
    dga-base
    EnTT
    expected
    FastDelegate
    flatbuffers
    fmt
    imgui
    json
    MathGeoLib
    sol2
    stb
    variant
    yojimbo
)
if(NOT EMSCRIPTEN)
    target_link_libraries(DwEngine
        luajit
    )
else()
    target_link_libraries(DwEngine
        lua
    )
endif()

# Tests.
enable_testing()

set(TEST_FILES
    core/io/FileSystemTest.cpp
    core/io/FileTest.cpp
    core/io/StringInputStreamTest.cpp
    testing/Testing.h)

add_executable(DwEngineTests ${TEST_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${TEST_FILES})
target_compile_features(DwEngineTests PUBLIC cxx_std_14)
target_link_libraries(DwEngineTests DwEngine gtest gtest_main)
target_include_directories(DwEngineTests SYSTEM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/testing)
set_target_properties(DwEngineTests PROPERTIES DEBUG_POSTFIX "")

include(GoogleTest)
gtest_add_tests(
    TARGET DwEngineTests
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
