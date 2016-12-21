# Dawn Engine
# Written by David Avedissian (c) 2012-2016 (git@davedissian.com)

find_package(OpenGL REQUIRED)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})
set(LIBS ${LIBS} ${OPENGL_LIBRARIES})

# Force everything to be statically linked
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Add bundled external libraries
set(INCLUDE_DIRS ${INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/include
    ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/3rdparty
    ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/3rdparty/khronos
    ${CMAKE_CURRENT_SOURCE_DIR}/bx/include
    ${CMAKE_CURRENT_SOURCE_DIR}/FastDelegate
    ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ImGui
    ${CMAKE_CURRENT_SOURCE_DIR}/json
    ${CMAKE_CURRENT_SOURCE_DIR}/ontology/include
    ${CMAKE_CURRENT_SOURCE_DIR}/sol
    ${CMAKE_CURRENT_SOURCE_DIR}/stb_image
    ${CMAKE_CURRENT_SOURCE_DIR}/tinyformat)
if(MSVC)
    set(INCLUDE_DIRS ${INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/bx/include/compat/msvc)
endif()


##############################################################
# External Projects
##############################################################

include(ExternalProject)

# External directories
set(EXTERNAL_PREFIX ${CMAKE_BINARY_DIR}/dependencies)
set(EXTERNAL_INSTALL ${EXTERNAL_PREFIX}/install)

# Arguments to be passed to all calls to CMake
set(EXTERNAL_ARGS
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} # CMake doesn't seem to do this automatically?
    -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL})
if(APPLE)
    set(EXTERNAL_ARGS ${EXTERNAL_ARGS}
        -DCMAKE_OSX_ARCHITECTURES=x86_64
        -DCMAKE_C_FLAGS=-fPIC\ -stdlib=libc++
        -DCMAKE_CXX_FLAGS=-fPIC\ -std=c++11\ -stdlib=libc++)
elseif(UNIX)
    set(EXTERNAL_ARGS ${EXTERNAL_ARGS}
        -DCMAKE_C_FLAGS=-fPIC
        -DCMAKE_CXX_FLAGS=-fPIC)
endif()

# General include directory for all external dependencies
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${EXTERNAL_INSTALL}/include)

# Bullet
ExternalProject_Add(Bullet
    PREFIX ${EXTERNAL_PREFIX}
    INSTALL_DIR ${EXTERNAL_INSTALL}
    GIT_REPOSITORY https://github.com/bulletphysics/bullet3.git
    UPDATE_COMMAND ""
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DBUILD_EXTRAS=0
        -DBUILD_BULLET2_DEMOS=0
        -DBUILD_BULLET3=0
        -DBUILD_UNIT_TESTS=0
        -DINSTALL_LIBS=1
        -DUSE_MSVC_RUNTIME_LIBRARY_DLL=1)
set_target_properties(Bullet PROPERTIES FOLDER "3rdparty")
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${EXTERNAL_INSTALL}/include/bullet)
if(WIN32)
    set(LIBS ${LIBS}
        debug ${EXTERNAL_INSTALL}/lib/BulletCollision_Debug.lib
        optimized ${EXTERNAL_INSTALL}/lib/BulletCollision.lib
        debug ${EXTERNAL_INSTALL}/lib/BulletDynamics_Debug.lib
        optimized ${EXTERNAL_INSTALL}/lib/BulletDynamics.lib
        debug ${EXTERNAL_INSTALL}/lib/LinearMath_Debug.lib
        optimized ${EXTERNAL_INSTALL}/lib/LinearMath.lib)
else()
    set(LIBS ${LIBS}
        ${EXTERNAL_INSTALL}/lib/libBulletCollision.a
        ${EXTERNAL_INSTALL}/lib/libBulletDynamics.a
        ${EXTERNAL_INSTALL}/lib/libLinearMath.a)
endif()

# MathGeoLib
ExternalProject_Add(MathGeoLib
    PREFIX ${EXTERNAL_PREFIX}
    INSTALL_DIR ${EXTERNAL_INSTALL}
    GIT_REPOSITORY https://github.com/davedissian/dawnengine-MathGeoLib.git
    UPDATE_COMMAND ""
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DBUILD_SHARED_LIBS=0)
set_target_properties(MathGeoLib PROPERTIES FOLDER "3rdparty")
set(INCLUDE_DIRS ${INCLUDE_DIRS}
    ${EXTERNAL_INSTALL}/include/MathGeoLib)
if(WIN32)
    set(LIBS ${LIBS}
        debug ${EXTERNAL_INSTALL}/lib/MathGeoLib_d.lib
        optimized ${EXTERNAL_INSTALL}/lib/MathGeoLib.lib)
else()
    set(LIBS ${LIBS}
        debug ${EXTERNAL_INSTALL}/lib/libMathGeoLib_d.a
        optimized ${EXTERNAL_INSTALL}/lib/libMathGeoLib.a)
endif()

# Luajit
ExternalProject_Add(Luajit
    PREFIX ${EXTERNAL_PREFIX}
    INSTALL_DIR ${EXTERNAL_INSTALL}
    GIT_REPOSITORY https://github.com/davedissian/dawnengine-luajit.git
    UPDATE_COMMAND ""
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DBUILD_SHARED_LIBS=0
        -DLUA_BUILD_WLUA=0)
if(WIN32)
    set(LIBS ${LIBS}
        ${EXTERNAL_INSTALL}/lib/lua.lib)
else()
    set(LIBS ${LIBS}
        ${EXTERNAL_INSTALL}/lib/liblua.a)
endif()
set_target_properties(Luajit PROPERTIES FOLDER "3rdparty")

# RakNet
ExternalProject_Add(RakNet
    PREFIX ${EXTERNAL_PREFIX}
    INSTALL_DIR ${EXTERNAL_INSTALL}
    GIT_REPOSITORY https://github.com/davedissian/dawnengine-RakNet.git
    UPDATE_COMMAND ""
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DRAKNET_ENABLE_DLL=0
        -DRAKNET_ENABLE_SAMPLES=0)
set_target_properties(RakNet PROPERTIES FOLDER "3rdparty")
if(WIN32)
    set(LIBS ${LIBS}
    	ws2_32.lib
        debug ${EXTERNAL_INSTALL}/lib/RakNetLibStatic_d.lib
        optimized ${EXTERNAL_INSTALL}/lib/RakNetLibStatic.lib)
else()
    set(LIBS ${LIBS}
    	pthread
        debug ${EXTERNAL_INSTALL}/lib/libRakNetLibStatic_d.a
        optimized ${EXTERNAL_INSTALL}/lib/libRakNetLibStatic.a)
endif()

set(EXTERNAL_DEPS Bullet MathGeoLib Luajit RakNet PARENT_SCOPE)


##############################################################
# Packaged Dependencies
##############################################################

# Build static libraries as PIC
if(UNIX) # TODO: If is Clang or GCC
    add_definitions(-fPIC)
endif()

# assimp
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(assimp)
target_include_directories(assimp PRIVATE ${INCLUDE_DIRS})
set_target_properties(assimp PROPERTIES FOLDER "3rdparty")
set(LIBS ${LIBS} assimp)

# bgfx
add_definitions(-DBGFX_CONFIG_RENDERER_OPENGL=33)
if(APPLE)
    add_library(bgfx bgfx/src/amalgamated.mm)
else()
    add_library(bgfx bgfx/src/amalgamated.cpp)
endif()
if(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(FOUNDATION_LIBRARY Foundation)
    target_link_libraries(bgfx ${COCOA_LIBRARY} ${FOUNDATION_LIBRARY})
endif()
target_include_directories(bgfx PRIVATE ${INCLUDE_DIRS})
set_target_properties(bgfx PROPERTIES FOLDER "3rdparty")
set(LIBS ${LIBS} bgfx)
if(WIN32)
    set(LIBS ${LIBS} psapi.lib)
endif()

# shaderc (part of bgfx)
set(BGFX_SHADERC_SRC
    bgfx/tools/shaderc/shaderc.cpp
    bgfx/tools/shaderc/shaderc_glsl.cpp
    bgfx/tools/shaderc/shaderc_hlsl.cpp)
file(GLOB BGFX_SHADERC_GLSL_OPTIMIZER_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    bgfx/3rdparty/fcpp/cpp*.c
    bgfx/3rdparty/glsl-optimizer/src/glsl/glcpp/*.c
    bgfx/3rdparty/glsl-optimizer/src/glsl/ast*.cpp
    bgfx/3rdparty/glsl-optimizer/src/glsl/builtin*.cpp
    bgfx/3rdparty/glsl-optimizer/src/glsl/glsl*.cpp
    bgfx/3rdparty/glsl-optimizer/src/glsl/hir*.cpp
    bgfx/3rdparty/glsl-optimizer/src/glsl/ir*.cpp
    bgfx/3rdparty/glsl-optimizer/src/glsl/link*.cpp
    bgfx/3rdparty/glsl-optimizer/src/glsl/loop*.cpp
    bgfx/3rdparty/glsl-optimizer/src/glsl/lower*.cpp
    bgfx/3rdparty/glsl-optimizer/src/glsl/opt*.cpp
    bgfx/3rdparty/glsl-optimizer/src/glsl/standalone_scaffolding.cpp
    bgfx/3rdparty/glsl-optimizer/src/glsl/s_expression.cpp
    bgfx/3rdparty/glsl-optimizer/src/glsl/*.c
    bgfx/3rdparty/glsl-optimizer/src/mesa/main/*.c
    bgfx/3rdparty/glsl-optimizer/src/mesa/program/*.c
    bgfx/3rdparty/glsl-optimizer/src/util/*.c)
add_executable(shaderc ${BGFX_SHADERC_SRC} ${BGFX_SHADERC_GLSL_OPTIMIZER_SRC})
enable_cpp11(shaderc)
target_link_libraries(shaderc bgfx)
target_include_directories(shaderc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/include
    ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/3rdparty/fcpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/3rdparty/glsl-optimizer/include
    ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/3rdparty/glsl-optimizer/src
    ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/3rdparty/glsl-optimizer/src/glsl
    ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/3rdparty/glsl-optimizer/src/mesa
    ${CMAKE_CURRENT_SOURCE_DIR}/bx/include)

# glfw
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
target_include_directories(glfw PRIVATE ${INCLUDE_DIRS})
set_target_properties(glfw PROPERTIES FOLDER "3rdparty")
set(LIBS ${LIBS} glfw)

# ImGui
add_library(ImGui
    ImGui/imgui.cpp
    ImGui/imgui_demo.cpp
    ImGui/imgui_draw.cpp)
target_include_directories(ImGui PRIVATE ${INCLUDE_DIRS})
set_target_properties(ImGui PROPERTIES FOLDER "3rdparty")
set(LIBS ${LIBS} ImGui)

# ontology
add_subdirectory(ontology)
enable_cpp14(ontology)
target_include_directories(ontology PRIVATE ${INCLUDE_DIRS})
set_target_properties(ontology PROPERTIES FOLDER "3rdparty")
set(LIBS ${LIBS} ontology)


##############################################################
# Push dependencies to the parent scope
##############################################################

set(INCLUDE_DIRS "${INCLUDE_DIRS}" PARENT_SCOPE)
set(LIBS "${LIBS}" PARENT_SCOPE)
