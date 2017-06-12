# Dawn Engine
# Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)

find_package(OpenGL REQUIRED)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})
set(LIBS ${LIBS} ${OPENGL_LIBRARIES})

# Force everything to be statically linked
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)


##############################################################
# External Projects
##############################################################

include(ExternalProject)

# External directories
set(EXTERNAL_PREFIX ${CMAKE_BINARY_DIR}/dependencies)
set(EXTERNAL_INSTALL ${EXTERNAL_PREFIX}/install)

# Arguments to be passed to all calls to CMake
set(EXTERNAL_ARGS
    -G ${CMAKE_GENERATOR}
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
    URL_MD5 c20ae75c10d1569cd6fa435eef079f56
    UPDATE_COMMAND ""
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DASSIMP_BUILD_ASSIMP_TOOLS=0
        -DASSIMP_BUILD_TESTS=0
)
set_target_properties(Assimp PROPERTIES EXCLUDE_FROM_ALL TRUE)

# Bullet
ExternalProject_Add(Bullet
    PREFIX ${EXTERNAL_PREFIX}
    INSTALL_DIR ${EXTERNAL_INSTALL}
    URL https://github.com/bulletphysics/bullet3/archive/2.86.zip
    URL_MD5 8b57278946186e9792824069637bbc97
    UPDATE_COMMAND ""
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DBUILD_EXTRAS=0
        -DBUILD_BULLET2_DEMOS=0
        -DBUILD_BULLET3=0
        -DBUILD_UNIT_TESTS=0
        -DINSTALL_LIBS=1
        -DUSE_MSVC_RUNTIME_LIBRARY_DLL=1
)
set_target_properties(Bullet PROPERTIES EXCLUDE_FROM_ALL TRUE)
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
        ${EXTERNAL_INSTALL}/lib/libBulletDynamics.a
        ${EXTERNAL_INSTALL}/lib/libBulletCollision.a
        ${EXTERNAL_INSTALL}/lib/libLinearMath.a)
endif()

# MathGeoLib
ExternalProject_Add(MathGeoLib
    PREFIX ${EXTERNAL_PREFIX}
    INSTALL_DIR ${EXTERNAL_INSTALL}
    URL https://github.com/davedissian/dawnengine-MathGeoLib/archive/master.zip
    URL_MD5 d1fd2ed6aa2284049234f377c6a6f1e9
    UPDATE_COMMAND ""
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DBUILD_SHARED_LIBS=0
)
set_target_properties(MathGeoLib PROPERTIES EXCLUDE_FROM_ALL TRUE)
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
    ExternalProject_Add(Luajit
        PREFIX ${EXTERNAL_PREFIX}
        URL http://luajit.org/download/LuaJIT-2.1.0-beta2.zip
        URL_MD5 b5d943c0174ca217736e2ddc2d9721c3
        BINARY_DIR ${EXTERNAL_PREFIX}/src/Luajit/src
        CONFIGURE_COMMAND ""
        BUILD_COMMAND msvcbuild.bat static
        INSTALL_COMMAND ""
        UPDATE_COMMAND ""
    )
else()
    ExternalProject_Add(Luajit
        PREFIX ${EXTERNAL_PREFIX}
        URL http://luajit.org/download/LuaJIT-2.1.0-beta2.zip
        URL_MD5 b5d943c0174ca217736e2ddc2d9721c3
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ""
        BUILD_COMMAND make PREFIX=${EXTERNAL_INSTALL} amalg
        INSTALL_COMMAND make PREFIX=${EXTERNAL_INSTALL} install
        UPDATE_COMMAND ""
    )
endif()
set_target_properties(Luajit PROPERTIES EXCLUDE_FROM_ALL TRUE)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${EXTERNAL_PREFIX}/src/Luajit/src)
if(WIN32)
    set(LIBS ${LIBS}
        ${EXTERNAL_PREFIX}/src/Luajit/src/lua51.lib)
else()
    set(LIBS ${LIBS}
        ${EXTERNAL_INSTALL}/lib/libluajit-5.1.a)
endif()

set(EXTERNAL_DEPS Assimp Bullet MathGeoLib Luajit PARENT_SCOPE)


##############################################################
# Packaged Dependencies
##############################################################

# Build static libraries as PIC
if(UNIX) # TODO: If is Clang or GCC
    add_definitions(-fPIC)
endif()

# bx (bgfx dependency)
add_library(bx INTERFACE)
target_include_directories(bx INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/bx/include)
if(MSVC)
    target_include_directories(bx INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/bx/include/compat/msvc)
endif()

# bgfx
add_definitions(-DBGFX_CONFIG_RENDERER_OPENGL=33)
if(APPLE)
    add_library(bgfx EXCLUDE_FROM_ALL bgfx/src/amalgamated.mm)
else()
    add_library(bgfx EXCLUDE_FROM_ALL bgfx/src/amalgamated.cpp)
endif()
target_include_directories(bgfx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/include)
target_include_directories(bgfx PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/3rdparty
    ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/3rdparty/khronos)
target_link_libraries(bgfx bx)
if(WIN32)
    # psapi.lib is used by bx::getProcessMemoryUsed().
    target_link_libraries(bgfx psapi.lib)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(FOUNDATION_LIBRARY Foundation)
    target_link_libraries(bgfx ${COCOA_LIBRARY} ${FOUNDATION_LIBRARY})
endif()
set(LIBS ${LIBS} bgfx)

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
target_link_libraries(shaderc bx)
target_include_directories(shaderc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/include
    ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/3rdparty/fcpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/3rdparty/glsl-optimizer/include
    ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/3rdparty/glsl-optimizer/src
    ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/3rdparty/glsl-optimizer/src/glsl
    ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/3rdparty/glsl-optimizer/src/mesa)

# enet
add_subdirectory(enet EXCLUDE_FROM_ALL)
set(LIBS ${LIBS} enet)

# FastDelegate
add_library(FastDelegate INTERFACE)
target_include_directories(FastDelegate INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/FastDelegate)
set(LIBS ${LIBS} FastDelegate)

# glfw
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw EXCLUDE_FROM_ALL)
target_include_directories(glfw PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include)
#target_include_directories(glfw PRIVATE ${INCLUDE_DIRS})
set(LIBS ${LIBS} glfw)

# gtest
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(gtest EXCLUDE_FROM_ALL)

# ImGui
add_library(ImGui EXCLUDE_FROM_ALL
    ImGui/imgui.cpp
    ImGui/imgui_demo.cpp
    ImGui/imgui_draw.cpp)
target_include_directories(ImGui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ImGui)
set(LIBS ${LIBS} ImGui)

# json
add_library(json INTERFACE)
target_include_directories(json INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/json)
set(LIBS ${LIBS} json)

# sol
add_library(sol INTERFACE)
target_include_directories(sol INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/sol)
set(LIBS ${LIBS} sol)

# stb_image
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/stb_image)
set(LIBS ${LIBS} stb_image)

# tinyformat
add_library(tinyformat INTERFACE)
target_include_directories(tinyformat INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/tinyformat)
set(LIBS ${LIBS} tinyformat)


##############################################################
# Push dependencies to the parent scope
##############################################################

set(INCLUDE_DIRS "${INCLUDE_DIRS}" PARENT_SCOPE)
set(LIBS "${LIBS}" PARENT_SCOPE)
