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
    -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL}
    # ExternalProject needs to have these passed in.
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
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

# Assimp
ExternalProject_Add(Assimp
    PREFIX ${EXTERNAL_PREFIX}
    INSTALL_DIR ${EXTERNAL_INSTALL}
    URL https://github.com/assimp/assimp/archive/v3.3.1.zip
    UPDATE_COMMAND ""
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DASSIMP_BUILD_ASSIMP_TOOLS=0
        -DASSIMP_BUILD_TESTS=0
)
set_target_properties(Assimp PROPERTIES FOLDER "contrib")

# Bullet
ExternalProject_Add(Bullet
    PREFIX ${EXTERNAL_PREFIX}
    INSTALL_DIR ${EXTERNAL_INSTALL}
    URL https://github.com/bulletphysics/bullet3/archive/2.85.1.zip
    UPDATE_COMMAND ""
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DBUILD_EXTRAS=0
        -DBUILD_BULLET2_DEMOS=0
        -DBUILD_BULLET3=0
        -DBUILD_UNIT_TESTS=0
        -DINSTALL_LIBS=1
        -DUSE_MSVC_RUNTIME_LIBRARY_DLL=1
)
set_target_properties(Bullet PROPERTIES FOLDER "contrib")
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
    URL https://github.com/davedissian/dawnengine-MathGeoLib/archive/master.zip
    UPDATE_COMMAND ""
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DBUILD_SHARED_LIBS=0
)
set_target_properties(MathGeoLib PROPERTIES FOLDER "contrib")
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
if (MSVC)
    set(LUAJIT_BUILD_CMD cd src && msvcbuild static)
endif()
ExternalProject_Add(Luajit
    PREFIX ${EXTERNAL_PREFIX}
    URL http://luajit.org/download/LuaJIT-2.1.0-beta2.zip
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${LUAJIT_BUILD_CMD}
    BUILD_IN_SOURCE 1
    BUILD_ALWAYS 0
)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${EXTERNAL_PREFIX}/src/Luajit/src)
if(WIN32)
    set(LIBS ${LIBS}
        ${EXTERNAL_PREFIX}/src/Luajit/src/lua51.lib)
else()
    set(LIBS ${LIBS}
        ${EXTERNAL_INSTALL}/lib/liblua.a)
endif()
set_target_properties(Luajit PROPERTIES FOLDER "contrib")

set(EXTERNAL_DEPS Assimp Bullet MathGeoLib Luajit PARENT_SCOPE)


##############################################################
# Packaged Dependencies
##############################################################

# Build static libraries as PIC
if(UNIX) # TODO: If is Clang or GCC
    add_definitions(-fPIC)
endif()

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
set_target_properties(bgfx PROPERTIES FOLDER "contrib")
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

# enet
add_subdirectory(enet)
set(LIBS ${LIBS} enet)
set_target_properties(enet PROPERTIES FOLDER "contrib")

# glfw
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
target_include_directories(glfw PRIVATE ${INCLUDE_DIRS})
set_target_properties(glfw PROPERTIES FOLDER "contrib")
set(LIBS ${LIBS} glfw)

# ImGui
add_library(ImGui
    ImGui/imgui.cpp
    ImGui/imgui_demo.cpp
    ImGui/imgui_draw.cpp)
target_include_directories(ImGui PRIVATE ${INCLUDE_DIRS})
set_target_properties(ImGui PROPERTIES FOLDER "contrib")
set(LIBS ${LIBS} ImGui)

# ontology
add_subdirectory(ontology)
enable_cpp14(ontology)
target_include_directories(ontology PRIVATE ${INCLUDE_DIRS})
set_target_properties(ontology PROPERTIES FOLDER "contrib")
set(LIBS ${LIBS} ontology)


##############################################################
# Push dependencies to the parent scope
##############################################################

set(INCLUDE_DIRS "${INCLUDE_DIRS}" PARENT_SCOPE)
set(LIBS "${LIBS}" PARENT_SCOPE)
