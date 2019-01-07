# Dawn Engine
# Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)

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
    FetchContent_Declare(
        PatchTool
        URL https://vorboss.dl.sourceforge.net/project/gnuwin32/patch/2.5.9-7/patch-2.5.9-7-bin.zip
        URL_MD5 b9c8b31d62f4b2e4f1887bbb63e8a905
    )
    FetchContent_GetProperties(PatchTool)
    if(NOT patchtool_POPULATED)
        FetchContent_Populate(PatchTool)
    endif()
    # Due to the fact Windows requires admin privileges to run any tool called "patch.exe", we need
    # to rename it to something else. See:
    # https://stackoverflow.com/questions/517257/how-do-i-apply-a-diff-patch-on-windows#comment32486422_14217009
    file(COPY ${patchtool_SOURCE_DIR}/bin/patch.exe DESTINATION ${patchtool_SOURCE_DIR}/bin/ptool)
    file(RENAME ${patchtool_SOURCE_DIR}/bin/ptool/patch.exe ${patchtool_SOURCE_DIR}/bin/ptool/ptool.exe)
    set(PATCH_TOOL ${patchtool_SOURCE_DIR}/bin/ptool/ptool.exe)
else()
    set(PATCH_TOOL patch)
endif()


##############################################################
# External Project Settings
##############################################################

# External directories
set(EXTERNAL_PREFIX ${CMAKE_BINARY_DIR}/dependencies)
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


##############################################################
# Dependencies
##############################################################

# Note that "set_property(TARGET <target> PROPERTY INTERFACE_INCLUDE_DIRECTORIES ...)" checks for the
# existence of the path. Therefore, we create the path beforehand.

# Assimp
if(WIN32)
    set(ASSIMP_PATCH_COMMAND)
else()
    set(ASSIMP_PATCH_COMMAND sed -e "/use_pkgconfig(UNZIP minizip)/s/^/#/g" -i CMakeLists.txt)
endif()
if(MSVC)
    set(ASSIMP_OUTPUT_LIBRARIES
        ${EXTERNAL_INSTALL}/lib/assimp-vc140-mt$<$<CONFIG:Debug>:_d>.lib
        ${EXTERNAL_PREFIX}/src/Assimp-external-build/contrib/irrXML/$<CONFIG>/IrrXML$<$<CONFIG:Debug>:_d>.lib
        ${EXTERNAL_PREFIX}/src/Assimp-external-build/contrib/zlib/$<CONFIG>/zlibstatic$<$<CONFIG:Debug>:d>.lib)
else()
    set(ASSIMP_OUTPUT_LIBRARIES
        ${EXTERNAL_INSTALL}/lib/libassimp$<$<CONFIG:Debug>:_d>.a
        ${EXTERNAL_PREFIX}/src/Assimp-external-build/contrib/irrXML/libIrrXML$<$<CONFIG:Debug>:_d>.a
        ${EXTERNAL_PREFIX}/src/Assimp-external-build/contrib/zlib/libzlibstatic$<$<CONFIG:Debug>:_d>.a)
endif()
ExternalProject_Add(Assimp-external
    PREFIX ${EXTERNAL_PREFIX}
    URL https://github.com/assimp/assimp/archive/v4.1.0.tar.gz
    URL_MD5 83b53a10c38d964bd1e69da0606e2727
    DOWNLOAD_NO_PROGRESS 1
    INSTALL_DIR ${EXTERNAL_INSTALL}
    PATCH_COMMAND ${PATCH_TOOL} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/assimp.diff
    UPDATE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install --config $<CONFIG>
    EXCLUDE_FROM_ALL 1
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DASSIMP_BUILD_ASSIMP_TOOLS=0
        -DASSIMP_BUILD_TESTS=0
        -DASSIMP_BUILD_ZLIB=1
        -DASSIMP_INSTALL_PDB=0
        -DCMAKE_DEBUG_POSTFIX=_d
)
add_library(Assimp INTERFACE)
add_dependencies(Assimp Assimp-external)
file(MAKE_DIRECTORY ${EXTERNAL_INSTALL}/include)
set_property(TARGET Assimp PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${EXTERNAL_INSTALL}/include)
set_property(TARGET Assimp PROPERTY INTERFACE_LINK_LIBRARIES ${ASSIMP_OUTPUT_LIBRARIES})
set(LIBS ${LIBS} Assimp)

# Bullet
FetchContent_Declare(
    bullet
    URL https://github.com/bulletphysics/bullet3/archive/2.88.tar.gz
    URL_MD5 d389e7235f00c66abf257bb7b21477ac
)
FetchContent_GetProperties(bullet)
if(NOT bullet_POPULATED)
    message(STATUS "Populating bullet")
    FetchContent_Populate(bullet)
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
    )
    # Other variables.
    mark_as_advanced(
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
    add_subdirectory(${bullet_SOURCE_DIR} ${bullet_BINARY_DIR} EXCLUDE_FROM_ALL)
    target_include_directories(BulletDynamics PUBLIC ${bullet_SOURCE_DIR}/src)
endif()
set(LIBS ${LIBS} BulletDynamics BulletCollision LinearMath)

# expected-lite
FetchContent_Declare(
    expected-lite
    URL https://github.com/martinmoene/expected-lite/archive/130322f172ba1e60f6ad14cddaecf5bbd6f68f05.tar.gz
    URL_MD5 b630c7bdb25f5317866f37a1de11658c
)
FetchContent_GetProperties(expected-lite)
if(NOT expected-lite_POPULATED)
    message(STATUS "Populating expected-lite")
    FetchContent_Populate(expected-lite)
    add_library(expected-lite INTERFACE)
    target_include_directories(expected-lite INTERFACE ${expected-lite_SOURCE_DIR}/include)
endif()
set(LIBS ${LIBS} expected-lite)

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
    URL https://github.com/google/flatbuffers/archive/v1.10.0.tar.gz
    URL_MD5 f7d19a3f021d93422b0bc287d7148cd2
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
ExternalProject_Add(flatbuffers-external
    PREFIX ${EXTERNAL_PREFIX}
    URL https://github.com/google/flatbuffers/archive/v1.10.0.tar.gz
    URL_MD5 f7d19a3f021d93422b0bc287d7148cd2
    DOWNLOAD_NO_PROGRESS 1
    INSTALL_DIR ${EXTERNAL_INSTALL}
    UPDATE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install --config $<CONFIG>
    EXCLUDE_FROM_ALL 1
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DFLATBUFFERS_BUILD_FLATC=0
        -DFLATBUFFERS_BUILD_FLATHASH=0
        -DFLATBUFFERS_BUILD_SHAREDLIB=0
        -DFLATBUFFERS_BUILD_TESTS=0
)
add_library(flatbuffers INTERFACE)
add_dependencies(flatbuffers flatbuffers-external flatc-external)
set_property(TARGET flatbuffers PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${EXTERNAL_INSTALL}/include)
if (UNIX AND NOT APPLE AND (NOT EXISTS "/etc/debian_version") AND NOT EMSCRIPTEN)
    set_property(TARGET flatbuffers PROPERTY INTERFACE_LINK_LIBRARIES
            ${EXTERNAL_INSTALL}/lib64/${CMAKE_STATIC_LIBRARY_PREFIX}flatbuffers${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
    set_property(TARGET flatbuffers PROPERTY INTERFACE_LINK_LIBRARIES
            ${EXTERNAL_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}flatbuffers${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()
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
        set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "" FORCE)
        add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR} EXCLUDE_FROM_ALL)
    else()
        add_library(glfw INTERFACE)
        target_include_directories(glfw INTERFACE ${glfw_SOURCE_DIR}/include)
        target_link_libraries(glfw INTERFACE "-s USE_GLFW=3" "-s USE_WEBGL2=1" "-s FULL_ES3=1")
    endif()
endif()
set(LIBS ${LIBS} glfw)

# glslang
ExternalProject_Add(glslang-external
    PREFIX ${EXTERNAL_PREFIX}
    URL https://github.com/KhronosGroup/glslang/archive/7.10.2984.tar.gz
    URL_MD5 865bad2658dd1d61f9c5583a4a1992e6
    DOWNLOAD_NO_PROGRESS 1
    INSTALL_DIR ${EXTERNAL_INSTALL}
    UPDATE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install --config $<CONFIG>
    EXCLUDE_FROM_ALL 1
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DBUILD_TESTING=0
        -DCMAKE_DEBUG_POSTFIX=d
        -DENABLE_GLSLANG_BINARIES=0
        -DENABLE_HLSL=0
)
add_library(glslang INTERFACE)
add_dependencies(glslang glslang-external)
file(MAKE_DIRECTORY ${EXTERNAL_INSTALL}/include/glslang/Public)
set_property(TARGET glslang PROPERTY INTERFACE_INCLUDE_DIRECTORIES
    ${EXTERNAL_INSTALL}/include/glslang/Public)
set_property(TARGET glslang PROPERTY INTERFACE_LINK_LIBRARIES
    ${EXTERNAL_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}SPIRV$<$<CONFIG:Debug>:d>${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${EXTERNAL_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}glslang$<$<CONFIG:Debug>:d>${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${EXTERNAL_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}OGLCompiler$<$<CONFIG:Debug>:d>${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${EXTERNAL_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}OSDependent$<$<CONFIG:Debug>:d>${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LIBS ${LIBS} glslang)

# googletest
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/3bedb5a9fc36769691c057b9e9414472035463b4.tar.gz
    URL_MD5 97af5dd741dd8dae3c00a0f22e736405
)
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    message(STATUS "Populating googletest")
    FetchContent_Populate(googletest)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# imgui
FetchContent_Declare(
    imgui
    URL https://github.com/ocornut/imgui/archive/v1.66b.tar.gz
    URL_MD5 57c57cbae62633fd22bcbbaa7f65554c
)
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    message(STATUS "Populating imgui")
    FetchContent_Populate(imgui)
    add_library(imgui EXCLUDE_FROM_ALL
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui.cpp)
    target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui_config)
    target_compile_definitions(imgui PUBLIC -DIMGUI_USER_CONFIG=<dw_imconfig.h>)
endif()
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
    set(LUAJIT_BUILD_COMMAND make PREFIX=${EXTERNAL_INSTALL} amalg)
    set(LUAJIT_INSTALL_COMMAND make PREFIX=${EXTERNAL_INSTALL} install)
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
ExternalProject_Add(MathGeoLib-external
    PREFIX ${EXTERNAL_PREFIX}
    URL https://github.com/juj/MathGeoLib/archive/01f92dd8fbd1fe2e405de713cfd985ff07d27dc9.tar.gz
    URL_MD5 bdab0f5b19661b7842501c738209e4fb
    DOWNLOAD_NO_PROGRESS 1
    INSTALL_DIR ${EXTERNAL_INSTALL}
    PATCH_COMMAND ${PATCH_TOOL} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/MathGeoLib.diff
    UPDATE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install --config $<CONFIG>
    EXCLUDE_FROM_ALL 1
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DCMAKE_DEBUG_POSTFIX=_d
        -DMATH_TESTS_EXECUTABLE=0
        -DVS_BUG=1
)
add_library(MathGeoLib INTERFACE)
add_dependencies(MathGeoLib MathGeoLib-external)
file(MAKE_DIRECTORY ${EXTERNAL_INSTALL}/include/MathGeoLib)
set_property(TARGET MathGeoLib PROPERTY INTERFACE_INCLUDE_DIRECTORIES
    ${EXTERNAL_INSTALL}/include/MathGeoLib)
if(EMSCRIPTEN)
    set_property(TARGET MathGeoLib PROPERTY INTERFACE_LINK_LIBRARIES
        ${EXTERNAL_INSTALL}/lib/MathGeoLib.bc)
else()
    set_property(TARGET MathGeoLib PROPERTY INTERFACE_LINK_LIBRARIES
        ${EXTERNAL_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}MathGeoLib$<$<CONFIG:Debug>:_d>${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()
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
ExternalProject_Add(SPIRVCross-external
    PREFIX ${EXTERNAL_PREFIX}
    URL https://github.com/KhronosGroup/SPIRV-Cross/archive/ed16b3e69985feaf565efbecea70a1cc2fca2a58.tar.gz
    URL_MD5 970fa559c40cee3ba603246a1696a6c9
    DOWNLOAD_NO_PROGRESS 1
    INSTALL_DIR ${EXTERNAL_INSTALL}
    UPDATE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install --config $<CONFIG>
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DCMAKE_DEBUG_POSTFIX=_d
)
set_target_properties(SPIRVCross-external PROPERTIES EXCLUDE_FROM_ALL TRUE)
add_library(SPIRVCross INTERFACE)
add_dependencies(SPIRVCross SPIRVCross-external)
set_property(TARGET SPIRVCross PROPERTY INTERFACE_LINK_LIBRARIES
    ${EXTERNAL_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}spirv-cross-core$<$<CONFIG:Debug>:_d>${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${EXTERNAL_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}spirv-cross-glsl$<$<CONFIG:Debug>:_d>${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${EXTERNAL_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}spirv-cross-hlsl$<$<CONFIG:Debug>:_d>${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LIBS ${LIBS} SPIRVCross)

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
    message(STATUS "Populating nanovg")
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
    URL https://github.com/c42f/tinyformat/archive/33d61f30f7c11dab2e4ed29e52e5e1cec0572feb.tar.gz
    URL_MD5 2200ef1627fddcbaa7637de71a4f37ce
)
FetchContent_GetProperties(tinyformat)
if(NOT tinyformat_POPULATED)
    message(STATUS "Populating tinyformat")
    FetchContent_Populate(tinyformat)
    add_library(tinyformat INTERFACE)
    target_include_directories(tinyformat INTERFACE ${tinyformat_SOURCE_DIR})
endif()
set(LIBS ${LIBS} tinyformat)

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
    ExternalProject_Add(yojimbo_mbedtls-external
        PREFIX ${EXTERNAL_PREFIX}
        URL https://github.com/ARMmbed/mbedtls/archive/mbedtls-2.16.0.tar.gz
        URL_MD5 331938f7bc54eabe1675d4cdf910f8d8
        DOWNLOAD_NO_PROGRESS 1
        INSTALL_DIR ${EXTERNAL_INSTALL}
        BUILD_COMMAND ""
        UPDATE_COMMAND ""
        INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install --config $<CONFIG>
        CMAKE_ARGS ${EXTERNAL_ARGS}
            -DCMAKE_DEBUG_POSTFIX=_d
            -DENABLE_TESTING=0
            -DENABLE_PROGRAMS=0
    )
    set_target_properties(yojimbo_mbedtls-external PROPERTIES EXCLUDE_FROM_ALL TRUE)
    add_library(yojimbo_mbedtls INTERFACE)
    add_dependencies(yojimbo_mbedtls yojimbo_mbedtls-external)
    file(MAKE_DIRECTORY ${EXTERNAL_INSTALL}/include)
    set_property(TARGET yojimbo_mbedtls PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${EXTERNAL_INSTALL}/include)
    set_property(TARGET yojimbo_mbedtls PROPERTY INTERFACE_LINK_LIBRARIES
        ${EXTERNAL_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}mbedtls$<$<CONFIG:Debug>:_d>${CMAKE_STATIC_LIBRARY_SUFFIX}
        ${EXTERNAL_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}mbedx509$<$<CONFIG:Debug>:_d>${CMAKE_STATIC_LIBRARY_SUFFIX}
        ${EXTERNAL_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}mbedcrypto$<$<CONFIG:Debug>:_d>${CMAKE_STATIC_LIBRARY_SUFFIX})

    # libsodium
    if(MSVC)
        ExternalProject_Add(yojimbo_sodium-external
            PREFIX ${EXTERNAL_PREFIX}
            URL https://download.libsodium.org/libsodium/releases/libsodium-1.0.15-msvc.zip
            URL_MD5 ddcf8c9ea41c035b9e609c324abd5973
            DOWNLOAD_NO_PROGRESS 1
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
            INSTALL_COMMAND ""
            UPDATE_COMMAND ""
        )
    elseif(NOT EMSCRIPTEN)
        ExternalProject_Add(yojimbo_sodium-external
            PREFIX ${EXTERNAL_PREFIX}
            URL https://github.com/jedisct1/libsodium/releases/download/1.0.15/libsodium-1.0.15.tar.gz
            URL_MD5 070373e73a0b10bd96f412e1732ebc42
            DOWNLOAD_NO_PROGRESS 1
            BUILD_IN_SOURCE 1
            CONFIGURE_COMMAND ./configure --prefix=${EXTERNAL_INSTALL} --enable-shared=no
            BUILD_COMMAND make
            INSTALL_COMMAND make install
            UPDATE_COMMAND ""
        )
    else()
        ExternalProject_Add(yojimbo_sodium-external
            PREFIX ${EXTERNAL_PREFIX}
            URL https://github.com/jedisct1/libsodium/releases/download/1.0.15/libsodium-1.0.15.tar.gz
            URL_MD5 070373e73a0b10bd96f412e1732ebc42
            DOWNLOAD_NO_PROGRESS 1
            BUILD_IN_SOURCE 1
            CONFIGURE_COMMAND emconfigure ./configure --prefix=${EXTERNAL_INSTALL} --enable-shared=no
            BUILD_COMMAND emmake make
            INSTALL_COMMAND emmake make install
            UPDATE_COMMAND ""
            )
    endif()
    set_target_properties(yojimbo_sodium-external PROPERTIES EXCLUDE_FROM_ALL TRUE)
    add_library(yojimbo_sodium INTERFACE)
    add_dependencies(yojimbo_sodium yojimbo_sodium-external)
    if(WIN32)
        file(MAKE_DIRECTORY ${EXTERNAL_PREFIX}/src/yojimbo_sodium-external/include)
        set_property(TARGET yojimbo_sodium PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${EXTERNAL_PREFIX}/src/yojimbo_sodium-external/include)
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
        set_property(TARGET yojimbo_sodium PROPERTY INTERFACE_LINK_LIBRARIES
            ${EXTERNAL_PREFIX}/src/yojimbo_sodium-external/${SODIUM_ARCH}/$<$<CONFIG:Debug>:Debug>$<$<NOT:$<CONFIG:Debug>>:Release>/${SODIUM_MSVC}/static/libsodium.lib)
    else()
        file(MAKE_DIRECTORY ${EXTERNAL_INSTALL}/include)
        set_property(TARGET yojimbo_sodium PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${EXTERNAL_INSTALL}/include)
        set_property(TARGET yojimbo_sodium PROPERTY INTERFACE_LINK_LIBRARIES ${EXTERNAL_INSTALL}/lib/libsodium.a)
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
    target_link_libraries(yojimbo yojimbo_mbedtls yojimbo_sodium)
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
