# Dawn Engine
# Written by David Avedissian (c) 2012-2019 (git@dga.dev)

include(FetchContent)
include(ExternalProject)

# Use GLVND if possible.
if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()

find_package(OpenGL REQUIRED)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})
set(LIBS ${LIBS} ${OPENGL_LIBRARIES})

# Force everything to be statically linked
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Build static libraries as PIC
if(UNIX) # TODO: If is Clang or GCC
    add_definitions(-fPIC)
endif()

# Patch tool
if(WIN32)
    set(PATCH_TOOL ${CMAKE_CURRENT_SOURCE_DIR}/../tools/ptool.exe)
else()
    set(PATCH_TOOL patch)
endif()


##############################################################
# External Project Settings
##############################################################

# External directories
set(EXTERNAL_PREFIX ${CMAKE_BINARY_DIR}/_deps/externalproject)
set(EXTERNAL_INSTALL ${EXTERNAL_PREFIX}/install)

# Arguments to be passed to all calls to CMake
set(HOST_EXTERNAL_ARGS
    -G ${CMAKE_GENERATOR}
    -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DBUILD_SHARED_LIBS=0)
if(APPLE)
    set(HOST_EXTERNAL_ARGS ${HOST_EXTERNAL_ARGS}
            -DCMAKE_OSX_ARCHITECTURES=x86_64
            -DCMAKE_C_FLAGS=-fPIC\ -stdlib=libc++\ -w
            -DCMAKE_CXX_FLAGS=-fPIC\ -std=c++11\ -stdlib=libc++\ -w)
elseif(UNIX)
    set(HOST_EXTERNAL_ARGS ${HOST_EXTERNAL_ARGS}
            -DCMAKE_C_FLAGS=-fPIC\ -w
            -DCMAKE_CXX_FLAGS=-fPIC\ -std=c++11\ -w)
endif()
set(EXTERNAL_ARGS ${HOST_EXTERNAL_ARGS}
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER})

# Commands to run when using Makefile projects.
if(EMSCRIPTEN)
    set(CONFIGURE emconfigure ./configure --prefix=${EXTERNAL_INSTALL})
    set(MAKE emmake make)
else()
    set(CONFIGURE ./configure --prefix=${EXTERNAL_INSTALL})
    if(APPLE)
        set(MAKE ${CMAKE_COMMAND} -E env MACOSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET} make)
    else()
        set(MAKE make)
    endif()
endif()


##############################################################
# Dependencies
##############################################################

# Note that "set_property(TARGET <target> PROPERTY INTERFACE_INCLUDE_DIRECTORIES ...)" checks for the
# existence of the path. Therefore, we create the path beforehand.

# Assimp
FetchContent_Declare(
    assimp
    URL https://github.com/assimp/assimp/archive/v4.1.0.tar.gz
    URL_MD5 83b53a10c38d964bd1e69da0606e2727
    PATCH_COMMAND ${PATCH_TOOL} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/assimp.diff
)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL_PDB OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(assimp)
target_include_directories(assimp PUBLIC
    ${assimp_SOURCE_DIR}/include
    ${assimp_BINARY_DIR}/include) # Generated assimp config.
set(LIBS ${LIBS} assimp)

# Bullet
FetchContent_Declare(
    bullet
    URL https://github.com/bulletphysics/bullet3/archive/2.88.tar.gz
    URL_MD5 d389e7235f00c66abf257bb7b21477ac
)
FetchContent_GetProperties(bullet)
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET3 OFF CACHE BOOL "" FORCE)
set(BUILD_CLSOCKET OFF CACHE BOOL "" FORCE)
set(BUILD_CPU_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(BUILD_ENET OFF CACHE BOOL "" FORCE)
set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_PYBULLET OFF CACHE BOOL "" FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BULLET2_MULTITHREADING OFF CACHE BOOL "" FORCE)
set(INSTALL_CMAKE_FILES OFF CACHE BOOL "" FORCE)
set(INSTALL_LIBS OFF CACHE BOOL "" FORCE)
set(USE_GRAPHICAL_BENCHMARK OFF CACHE BOOL "" FORCE)
set(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE BOOL "" FORCE)
set(USE_GLUT OFF CACHE BOOL "" FORCE)
set(USE_SOFT_BODY_MULTI_BODY_DYNAMICS_WORLD OFF CACHE BOOL "" FORCE)
# Mark variables above as advanced.
mark_as_advanced(
    BUILD_BULLET2_DEMOS
    BUILD_BULLET3
    BUILD_CLSOCKET
    BUILD_CPU_DEMOS
    BUILD_EXTRAS
    BUILD_ENET
    BUILD_OPENGL3_DEMOS
    BUILD_PYBULLET
    BUILD_UNIT_TESTS
    BULLET2_MULTITHREADING
    INSTALL_CMAKE_FILES
    INSTALL_LIBS
    USE_GRAPHICAL_BENCHMARK
    USE_MSVC_RUNTIME_LIBRARY_DLL
    USE_GLUT
    USE_SOFT_BODY_MULTI_BODY_DYNAMICS_WORLD

    # Other variables introduced by bullet.
    CLSOCKET_DEP_ONLY
    CLSOCKET_SHARED
    LIBRARY_OUTPUT_PATH
    USE_CUSTOM_VECTOR_MATH
    USE_DOUBLE_PRECISION
    USE_MSVC_AVX
    USE_MSVC_COMDAT_FOLDING
    USE_MSVC_DISABLE_RTTI
    USE_MSVC_EXEPTIONS
    USE_MSVC_FAST_FLOATINGPOINT
    USE_MSVC_FUNCTION_LEVEL_LINKING
    USE_MSVC_INCREMENTAL_LINKING
    USE_MSVC_STRING_POOLING
)
FetchContent_MakeAvailable(bullet)
target_include_directories(BulletDynamics PUBLIC ${bullet_SOURCE_DIR}/src)
set(LIBS ${LIBS} BulletDynamics BulletCollision LinearMath)

# concurrent queue
FetchContent_Declare(
    concurrentqueue
    URL https://github.com/cameron314/concurrentqueue/archive/dea078cf5b6e742cd67a0d725e36f872feca4de4.tar.gz
    URL_MD5 02b83675f33f31dd070f386fc8f10ac9
)
FetchContent_MakeAvailable(concurrentqueue)
add_library(concurrentqueue INTERFACE)
target_include_directories(concurrentqueue INTERFACE ${concurrentqueue_SOURCE_DIR})
set(LIBS ${LIBS} concurrentqueue)

# entt
FetchContent_Declare(
    entt
    URL https://github.com/skypjack/entt/archive/v3.1.1.tar.gz
    URL_MD5 b450b8fdad7f843d2b2dbad77ac61a3b
)
set(USE_LIBCPP OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(entt)
set(LIBS ${LIBS} EnTT)

# expected
FetchContent_Declare(
    expected
    URL https://github.com/TartanLlama/expected/archive/fd96e4545cc50ddfbbead177ef6594b12b210d62.zip
    URL_MD5 8e0769ee95b95f4aa58d5a37d51d9bad
)
if(NOT expected_POPULATED)
    FetchContent_Populate(expected)
    add_library(expected INTERFACE)
    target_include_directories(expected INTERFACE ${expected_SOURCE_DIR}/include)
endif()
set(LIBS ${LIBS} expected)

# FastDelegate
add_library(FastDelegate INTERFACE)
target_include_directories(FastDelegate INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/FastDelegate)
set(LIBS ${LIBS} FastDelegate)

# flatbuffers
#
# We build flatbuffers twice, first with the host toolchain to get flatc, then again against the target toolchain
# without flatc.
ExternalProject_Add(flatc-external
    PREFIX ${EXTERNAL_PREFIX}
    URL https://github.com/google/flatbuffers/archive/v1.11.0.tar.gz
    URL_MD5 02c64880acb89dbd57eebacfd67200d8
    DOWNLOAD_NO_PROGRESS 1
    INSTALL_DIR ${EXTERNAL_INSTALL}
    UPDATE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target flatc --config $<CONFIG>
    EXCLUDE_FROM_ALL 1
    CMAKE_ARGS ${HOST_EXTERNAL_ARGS}
        -DFLATBUFFERS_BUILD_FLATC=1
        -DFLATBUFFERS_BUILD_FLATHASH=0
        -DFLATBUFFERS_BUILD_SHAREDLIB=0
        -DFLATBUFFERS_BUILD_TESTS=0
)
if(MSVC)
    set(FLATBUFFERS_FLATC_EXECUTABLE ${EXTERNAL_PREFIX}/src/flatc-external-build/$<CONFIG>/flatc PARENT_SCOPE)
else()
    set(FLATBUFFERS_FLATC_EXECUTABLE ${EXTERNAL_PREFIX}/src/flatc-external-build/flatc PARENT_SCOPE)
endif()
FetchContent_Declare(flatbuffers
    PREFIX ${EXTERNAL_PREFIX}
    URL https://github.com/google/flatbuffers/archive/v1.11.0.tar.gz
    URL_MD5 02c64880acb89dbd57eebacfd67200d8
)
set(FLATBUFFERS_BUILD_FLATC OFF CACHE BOOL "" FORCE)
set(FLATBUFFERS_BUILD_FLATHASH OFF CACHE BOOL "" FORCE)
set(FLATBUFFERS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(flatbuffers)
add_dependencies(flatbuffers flatc-external)
set(LIBS ${LIBS} flatbuffers)

# glad
if(NOT EMSCRIPTEN)
    add_library(glad EXCLUDE_FROM_ALL glad/src/glad.c)
    target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/include)
    set(LIBS ${LIBS} glad)
endif()

# glfw
FetchContent_Declare(
    glfw
    URL https://github.com/glfw/glfw/archive/c90c7b97109db909577e3bf540b5f884422b7e14.tar.gz
    URL_MD5 326ceda8f5184e3e79625c6d2268a3c5
)
FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
    message(STATUS "Populating glfw")
    FetchContent_Populate(glfw)
    if(NOT EMSCRIPTEN)
        set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
        set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
        set(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE BOOL "" FORCE)
        add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR} EXCLUDE_FROM_ALL)
    else()
        add_library(glfw INTERFACE)
        target_include_directories(glfw INTERFACE ${glfw_SOURCE_DIR}/include)
        target_link_libraries(glfw INTERFACE "-s USE_GLFW=3" "-s USE_WEBGL2=1" "-s FULL_ES3=1")
    endif()
endif()
set(LIBS ${LIBS} glfw)

# glslang
FetchContent_Declare(
    glslang
    URL https://github.com/KhronosGroup/glslang/archive/7.10.2984.tar.gz
    URL_MD5 865bad2658dd1d61f9c5583a4a1992e6
)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)
set(ENABLE_HLSL OFF CACHE BOOL "" FORCE)
mark_as_advanced(
        BUILD_TESTING
        ENABLE_GLSLANG_BINARIES
        ENABLE_HLSL
)
FetchContent_MakeAvailable(glslang)
set(LIBS ${LIBS} glslang SPIRV)

# googletest
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/release-1.10.0.tar.gz
    URL_MD5 ecd1fa65e7de707cd5c00bdac56022cd
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Policies needed by gtest
cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0057 NEW)

# imgui
FetchContent_Declare(
    imgui
    URL https://github.com/ocornut/imgui/archive/v1.66b.tar.gz
    URL_MD5 57c57cbae62633fd22bcbbaa7f65554c
)
FetchContent_MakeAvailable(imgui)
add_library(imgui EXCLUDE_FROM_ALL
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui.cpp)
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui_config)
target_compile_definitions(imgui PUBLIC -DIMGUI_USER_CONFIG=<dw_imconfig.h>)
set(LIBS ${LIBS} imgui)

# json
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.5.0/include.zip
    URL_MD5 85f46eac58a2cca028d23b227c66cf13
)
FetchContent_GetProperties(json)
if(NOT json_POPULATED)
    message(STATUS "Populating json")
    FetchContent_Populate(json)
    add_library(json INTERFACE)
    target_include_directories(json INTERFACE ${json_SOURCE_DIR})
endif()
set(LIBS ${LIBS} json)

# Luajit
if(MSVC)
    set(LUAJIT_BUILD_COMMAND cd src COMMAND msvcbuild.bat static)
    set(LUAJIT_INSTALL_COMMAND
        ${CMAKE_COMMAND} -E copy
            ${EXTERNAL_PREFIX}/src/luajit-external-build/src/lua51.lib
            ${EXTERNAL_INSTALL}/lib/luajit-5.1.lib)
else()
    set(LUAJIT_BUILD_COMMAND ${MAKE} PREFIX=${EXTERNAL_INSTALL} amalg)
    set(LUAJIT_INSTALL_COMMAND ${MAKE} PREFIX=${EXTERNAL_INSTALL} install)
endif()
ExternalProject_Add(luajit-external
    PREFIX ${EXTERNAL_PREFIX}
    URL https://luajit.org/download/LuaJIT-2.0.5.tar.gz
    URL_MD5 48353202cbcacab84ee41a5a70ea0a2c
    DOWNLOAD_NO_PROGRESS 1
    # This is needed to avoid nuking the build files every time the DOWNLOAD_COMMAND
    # is run (and files are copied from external/luajit). This causes LJ to rebuild
    # every time in VS without specifying this CONFIGURE_COMMAND.
    CONFIGURE_COMMAND
        ${CMAKE_COMMAND} -E copy_directory
            ${EXTERNAL_PREFIX}/src/luajit-external
            ${EXTERNAL_PREFIX}/src/luajit-external-build
    UPDATE_COMMAND ""
    BUILD_COMMAND ${LUAJIT_BUILD_COMMAND}
    BUILD_BYPRODUCTS
        ${EXTERNAL_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}luajit-5.1${CMAKE_STATIC_LIBRARY_SUFFIX}
    INSTALL_COMMAND ${LUAJIT_INSTALL_COMMAND}
    EXCLUDE_FROM_ALL 1
)
add_library(luajit INTERFACE)
add_dependencies(luajit luajit-external)
file(MAKE_DIRECTORY ${EXTERNAL_PREFIX}/src/luajit-external/src)
set_property(TARGET luajit PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${EXTERNAL_PREFIX}/src/luajit-external/src)
set_property(TARGET luajit PROPERTY INTERFACE_LINK_LIBRARIES
    ${EXTERNAL_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}luajit-5.1${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LIBS ${LIBS} luajit)

# MathGeoLib
FetchContent_Declare(
    mathgeolib
    URL https://github.com/juj/MathGeoLib/archive/01f92dd8fbd1fe2e405de713cfd985ff07d27dc9.tar.gz
    URL_MD5 bdab0f5b19661b7842501c738209e4fb
    PATCH_COMMAND ${PATCH_TOOL} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/MathGeoLib.diff
)
set(VS_BUG ON CACHE BOOL "" FORCE)
set(MATH_TESTS_EXECUTABLE OFF CACHE BOOL "" FORCE)
set(INSTALL_CMAKE_DIR ${CMAKE_BINARY_DIR}/_deps/mathgeolib-build CACHE BOOL "" FORCE)
set(INSTALL_INCLUDE_DIR ${CMAKE_BINARY_DIR}/_deps/mathgeolib-build CACHE BOOL "" FORCE)
mark_as_advanced(
    VS_BUG
    MATH_TESTS_EXECUTABLE
    GENERATE_ASM_LISTING
    INSTALL_CMAKE_DIR
    INSTALL_INCLUDE_DIR
)
FetchContent_MakeAvailable(mathgeolib)
target_include_directories(MathGeoLib PUBLIC $<BUILD_INTERFACE:${mathgeolib_SOURCE_DIR}/src>)
set(LIBS ${LIBS} MathGeoLib)

# sol2
FetchContent_Declare(
    sol2
    URL https://github.com/ThePhD/sol2/archive/v2.20.6.tar.gz
    URL_MD5 f515a1e7aa65087ebcfd3c343f4b3b34
)
FetchContent_GetProperties(sol2)
if(NOT sol2_POPULATED)
    message(STATUS "Populating sol2")
    FetchContent_Populate(sol2)
    add_library(sol2 INTERFACE)
    target_include_directories(sol2 INTERFACE ${sol2_SOURCE_DIR}/single/sol)
endif()
set(LIBS ${LIBS} sol2)

# SPIRV-Cross
FetchContent_Declare(
    spirv-cross
    URL https://github.com/KhronosGroup/SPIRV-Cross/archive/ed16b3e69985feaf565efbecea70a1cc2fca2a58.tar.gz
    URL_MD5 970fa559c40cee3ba603246a1696a6c9
)
FetchContent_GetProperties(spirv-cross)
if(NOT spirv-cross_POPULATED)
    message(STATUS "Populating spirv-cross")
    FetchContent_Populate(spirv-cross)
    add_subdirectory(${spirv-cross_SOURCE_DIR} ${spirv-cross_BINARY_DIR} EXCLUDE_FROM_ALL)
    target_include_directories(spirv-cross-core PUBLIC $<BUILD_INTERFACE:${spirv-cross_SOURCE_DIR}/include/spirv_cross>)
endif()
set(LIBS ${LIBS} spirv-cross-glsl spirv-cross-hlsl)

# stb
FetchContent_Declare(
    stb
    URL https://github.com/nothings/stb/archive/e6afb9cbae4064da8c3e69af3ff5c4629579c1d2.tar.gz
    URL_MD5 e52f1d8c18612d4d7a2eb1c5e2d7ed0f
)
FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
    message(STATUS "Populating stb")
    FetchContent_Populate(stb)
    add_library(stb INTERFACE)
    target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})
endif()
set(LIBS ${LIBS} stb)

# nanovg
FetchContent_Declare(
    nanovg
    URL https://github.com/memononen/nanovg/archive/f4069e6a1ad5da430fb0a9c57476d5ddc2ff89b2.tar.gz
    URL_MD5 45e724d3bcb058ebc8001f1a82ac9334
)
FetchContent_GetProperties(nanovg)
if(NOT nanovg_POPULATED)
    FetchContent_Populate(nanovg)
    add_library(nanovg EXCLUDE_FROM_ALL
        ${nanovg_SOURCE_DIR}/src/fontstash.h
        ${nanovg_SOURCE_DIR}/src/nanovg.c
        ${nanovg_SOURCE_DIR}/src/nanovg.h)
    target_include_directories(nanovg PUBLIC ${nanovg_SOURCE_DIR}/src)
endif()

# tinyformat
FetchContent_Declare(
    tinyformat
    URL https://github.com/c42f/tinyformat/archive/v2.2.0.tar.gz
    URL_MD5 a88a198f7ea99eeed9ed2169a977f0b3
)
FetchContent_GetProperties(tinyformat)
if(NOT tinyformat_POPULATED)
    FetchContent_Populate(tinyformat)
    add_library(tinyformat INTERFACE)
    target_include_directories(tinyformat INTERFACE ${tinyformat_SOURCE_DIR})
endif()
set(LIBS ${LIBS} tinyformat)

# variant
FetchContent_Declare(
    variant
    URL https://github.com/mpark/variant/archive/v1.4.0.tar.gz
    URL_MD5 d5074ffb30167255f8c70e4ac0af49cf
)
FetchContent_GetProperties(variant)
if(NOT variant_POPULATED)
    FetchContent_Populate(variant)
    add_library(variant INTERFACE)
    target_include_directories(variant INTERFACE ${variant_SOURCE_DIR}/include)
endif()
set(LIBS ${LIBS} variant)

# yojimbo
FetchContent_Declare(
    yojimbo
    GIT_REPOSITORY https://github.com/networkprotocol/yojimbo.git
    GIT_TAG ce9573cdc1ceafec36366f56d7db7f386f71df01
)
FetchContent_GetProperties(yojimbo)
if(NOT yojimbo_POPULATED)
    message(STATUS "Populating yojimbo")
    FetchContent_Populate(yojimbo)

    # mbedtls
    FetchContent_Declare(
        mbedtls
        URL https://github.com/ARMmbed/mbedtls/archive/mbedtls-2.16.0.tar.gz
        URL_MD5 331938f7bc54eabe1675d4cdf910f8d8
    )
    set(ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(ENABLE_PROGRAMS OFF CACHE BOOL "" FORCE)
    set(USE_STATIC_MBEDTLS_LIBRARY ON CACHE BOOL "" FORCE)
    set(USE_SHARED_MBEDTLS_LIBRARY OFF CACHE BOOL "" FORCE)
    mark_as_advanced(
        ENABLE_TESTING
        ENABLE_PROGRAMS
        ENABLE_ZLIB_SUPPORT
        INSTALL_MBEDTLS_HEADERS
        LINK_WITH_PTHREAD
        UNSAFE_BUILD
        USE_PKCS11_HELPER_LIBRARY
        USE_SHARED_MBEDTLS_LIBRARY
        USE_STATIC_MBEDTLS_LIBRARY
    )
    FetchContent_MakeAvailable(mbedtls)
    
    # sodium
    if(MSVC)
        FetchContent_Declare(
            sodium
            URL https://download.libsodium.org/libsodium/releases/libsodium-1.0.17-msvc.zip
            URL_MD5 a90f193ac3ce21e9578aae9b9ff10343
        )
        FetchContent_MakeAvailable(sodium)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(SODIUM_ARCH x64)
        elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
            set(SODIUM_ARCH Win32)
        else()
            message(FATAL_ERROR "Unsupported architecture. CMAKE_SIZEOF_VOID_P is ${CMAKE_SIZEOF_VOID_P}")
        endif()
        if(NOT(MSVC_VERSION LESS 1910))
            # Visual Studio 15.0 (2017)
            set(SODIUM_MSVC v141)
        elseif(NOT(MSVC_VERSION LESS 1900))
            # Visual Studio 14.0 (2015)
            set(SODIUM_MSVC v140)
        else()
            message(FATAL_ERROR "Unsupported MSVC version: ${MSVC_VERSION}")
        endif()
        set_property(TARGET sodium PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${sodium_SOURCE_DIR}/include)
        set_property(TARGET sodium PROPERTY INTERFACE_LINK_LIBRARIES
            ${sodium_SOURCE_DIR}/${SODIUM_ARCH}/$<$<CONFIG:Debug>:Debug>$<$<NOT:$<CONFIG:Debug>>:Release>/${SODIUM_MSVC}/static/libsodium.lib)
        add_library(sodium INTERFACE)
    else()
        ExternalProject_Add(sodium-external
            PREFIX ${EXTERNAL_PREFIX}
            URL https://github.com/jedisct1/libsodium/releases/download/1.0.17/libsodium-1.0.17.tar.gz
            URL_MD5 0f71e2680187a1558b5461e6879342c5
            DOWNLOAD_NO_PROGRESS 1
            BUILD_IN_SOURCE 1
            CONFIGURE_COMMAND ${CONFIGURE} --enable-shared=no
            BUILD_COMMAND ${MAKE}
            BUILD_BYPRODUCTS ${EXTERNAL_INSTALL}/lib/libsodium.a
            INSTALL_COMMAND ${MAKE} install
            UPDATE_COMMAND ""
        )
        set_target_properties(sodium-external PROPERTIES EXCLUDE_FROM_ALL TRUE)
        add_library(sodium INTERFACE)
        add_dependencies(sodium sodium-external)
        file(MAKE_DIRECTORY ${EXTERNAL_INSTALL}/include)
        set_property(TARGET sodium PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${EXTERNAL_INSTALL}/include)
        set_property(TARGET sodium PROPERTY INTERFACE_LINK_LIBRARIES ${EXTERNAL_INSTALL}/lib/libsodium.a)
    endif()

    # yojimbo
    add_library(yojimbo EXCLUDE_FROM_ALL
        ${yojimbo_SOURCE_DIR}/yojimbo.cpp
        ${yojimbo_SOURCE_DIR}/tlsf/tlsf.c
        ${yojimbo_SOURCE_DIR}/netcode.io/netcode.c
        ${yojimbo_SOURCE_DIR}/reliable.io/reliable.c)
    target_include_directories(yojimbo PUBLIC
        ${yojimbo_SOURCE_DIR})
    target_include_directories(yojimbo PRIVATE
        ${yojimbo_SOURCE_DIR}/netcode.io
        ${yojimbo_SOURCE_DIR}/reliable.io)
    target_link_libraries(yojimbo mbedtls sodium)
    target_include_directories(yojimbo PRIVATE ${mbedtls_SOURCE_DIR}/include)
    if(EMSCRIPTEN)
        target_compile_definitions(yojimbo PRIVATE -D__linux=1)
    endif()
endif()
set(LIBS ${LIBS} yojimbo)

##############################################################
# Push dependencies to the parent scope
##############################################################

set(INCLUDE_DIRS "${INCLUDE_DIRS}" PARENT_SCOPE)
set(LIBS "${LIBS}" PARENT_SCOPE)
