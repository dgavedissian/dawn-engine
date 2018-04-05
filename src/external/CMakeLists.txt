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
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DBUILD_SHARED_LIBS=0)
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

# Note that "set_property(TARGET <target> PROPERTY INTERFACE_INCLUDE_DIRECTORIES ...)" checks for the
# existence of the path. Therefore, we create the path beforehand.

# Assimp
if(WIN32)
    set(ASSIMP_PATCH_COMMAND)
else()
    set(ASSIMP_PATCH_COMMAND sed -e "/use_pkgconfig(UNZIP minizip)/s/^/#/g" -i CMakeLists.txt)
endif()
ExternalProject_Add(Assimp-external
    PREFIX ${EXTERNAL_PREFIX}
    # As we need to patch the source, we make a copy using URL instead.
    URL ${CMAKE_CURRENT_SOURCE_DIR}/assimp
    INSTALL_DIR ${EXTERNAL_INSTALL}
    PATCH_COMMAND ${ASSIMP_PATCH_COMMAND}
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
set_property(TARGET Assimp PROPERTY INTERFACE_LINK_LIBRARIES
    ${EXTERNAL_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}assimp$<$<PLATFORM_ID:Windows>:-vc140-mt>$<$<CONFIG:Debug>:_d>${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${EXTERNAL_PREFIX}/src/Assimp-external-build/contrib/irrXML/${CMAKE_STATIC_LIBRARY_PREFIX}IrrXML$<$<CONFIG:Debug>:_d>${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${EXTERNAL_PREFIX}/src/Assimp-external-build/contrib/zlib/${CMAKE_STATIC_LIBRARY_PREFIX}zlibstatic$<$<CONFIG:Debug>:_d>${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LIBS ${LIBS} Assimp)

# Bullet
ExternalProject_Add(Bullet-external
    PREFIX ${EXTERNAL_PREFIX}
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bullet
    INSTALL_DIR ${EXTERNAL_INSTALL}
    DOWNLOAD_COMMAND ""
    UPDATE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install --config $<CONFIG>
    EXCLUDE_FROM_ALL 1
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DBUILD_BULLET2_DEMOS=0
        -DBUILD_BULLET3=0
        -DBUILD_CLSOCKET=0
        -DBUILD_CPU_DEMOS=0
        -DBUILD_EXTRAS=0
        -DBUILD_ENET=0
        -DBUILD_PYBULLET=0
        -DBUILD_UNIT_TESTS=0
        -DINSTALL_CMAKE_FILES=0
        -DINSTALL_LIBS=1
        -DUSE_MSVC_RUNTIME_LIBRARY_DLL=1
        # Needed on Linux
        -DCMAKE_RELWITHDEBINFO_POSTFIX=
        -DCMAKE_DEBUG_POSTFIX=_d
)
add_library(Bullet INTERFACE)
add_dependencies(Bullet Bullet-external)
file(MAKE_DIRECTORY ${EXTERNAL_INSTALL}/include/bullet)
set_property(TARGET Bullet PROPERTY INTERFACE_INCLUDE_DIRECTORIES
    ${EXTERNAL_INSTALL}/include/bullet)
set_property(TARGET Bullet PROPERTY INTERFACE_LINK_LIBRARIES
    ${EXTERNAL_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}BulletDynamics$<$<CONFIG:Debug>:_d>${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${EXTERNAL_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}BulletCollision$<$<CONFIG:Debug>:_d>${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${EXTERNAL_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}LinearMath$<$<CONFIG:Debug>:_d>${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LIBS ${LIBS} Bullet)

# MathGeoLib
ExternalProject_Add(MathGeoLib-external
    PREFIX ${EXTERNAL_PREFIX}
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/MathGeoLib
    INSTALL_DIR ${EXTERNAL_INSTALL}
    DOWNLOAD_COMMAND ""
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
set_property(TARGET MathGeoLib PROPERTY INTERFACE_LINK_LIBRARIES
    ${EXTERNAL_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}MathGeoLib$<$<CONFIG:Debug>:_d>${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LIBS ${LIBS} MathGeoLib)

# Luajit
if(MSVC)
    set(LUAJIT_BUILD_COMMAND msvcbuild.bat static)
    set(LUAJIT_INSTALL_COMMAND "")
else()
    set(LUAJIT_BUILD_COMMAND make PREFIX=${EXTERNAL_INSTALL} amalg)
    set(LUAJIT_INSTALL_COMMAND make PREFIX=${EXTERNAL_INSTALL} install)
endif()
ExternalProject_Add(Luajit-external
    PREFIX ${EXTERNAL_PREFIX}
    URL ${CMAKE_CURRENT_SOURCE_DIR}/luajit
    CONFIGURE_COMMAND ""
    BUILD_IN_SOURCE 1
    BUILD_COMMAND ${LUAJIT_BUILD_COMMAND}
    INSTALL_COMMAND ${LUAJIT_INSTALL_COMMAND}
    UPDATE_COMMAND ""
    EXCLUDE_FROM_ALL 1
)
add_library(Luajit INTERFACE)
add_dependencies(Luajit Luajit-external)
file(MAKE_DIRECTORY ${EXTERNAL_PREFIX}/src/Luajit-external/src)
set_property(TARGET Luajit PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${EXTERNAL_PREFIX}/src/Luajit-external/src)
if(WIN32)
    set_property(TARGET Luajit PROPERTY INTERFACE_LINK_LIBRARIES
        ${EXTERNAL_PREFIX}/src/Luajit-external/src/lua51.lib)
else()
    set_property(TARGET Luajit PROPERTY INTERFACE_LINK_LIBRARIES
        ${EXTERNAL_INSTALL}/lib/libluajit-5.1.a)
endif()
set(LIBS ${LIBS} Luajit)


##############################################################
# Packaged Dependencies
##############################################################

# Build static libraries as PIC
if(UNIX) # TODO: If is Clang or GCC
    add_definitions(-fPIC)
endif()

# any-lite
add_library(any-lite INTERFACE)
target_include_directories(any-lite INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/any-lite/include)
set(LIBS ${LIBS} any-lite)

# expected-lite
add_library(expected-lite INTERFACE)
target_include_directories(expected-lite INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/expected-lite/include)
set(LIBS ${LIBS} expected-lite)

# FastDelegate
add_library(FastDelegate INTERFACE)
target_include_directories(FastDelegate INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/FastDelegate)
set(LIBS ${LIBS} FastDelegate)

# gl3w
add_library(gl3w EXCLUDE_FROM_ALL gl3w/gl3w.c gl3w/GL/gl3w.h gl3w/GL/glcorearb.h)
target_include_directories(gl3w PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/gl3w)
set(LIBS ${LIBS} gl3w)

# glfw
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_USE_CHDIR OFF CACHE BOOL "" FORCE)
set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw EXCLUDE_FROM_ALL)
target_include_directories(glfw PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include)
set(LIBS ${LIBS} glfw)

# glslang
ExternalProject_Add(glslang-external
    PREFIX ${EXTERNAL_PREFIX}
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glslang
    INSTALL_DIR ${EXTERNAL_INSTALL}
    DOWNLOAD_COMMAND ""
    UPDATE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install --config $<CONFIG>
    EXCLUDE_FROM_ALL 1
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DBUILD_TESTING=0
        -DCMAKE_DEBUG_POSTFIX=_d
        -DENABLE_GLSLANG_BINARIES=0
        -DENABLE_HLSL=0
)
add_library(glslang INTERFACE)
add_dependencies(glslang glslang-external)
file(MAKE_DIRECTORY ${EXTERNAL_INSTALL}/include/glslang/Public)
set_property(TARGET glslang PROPERTY INTERFACE_INCLUDE_DIRECTORIES
    ${EXTERNAL_INSTALL}/include/glslang/Public)
set_property(TARGET glslang PROPERTY INTERFACE_LINK_LIBRARIES
    ${EXTERNAL_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}SPIRV$<$<CONFIG:Debug>:_d>${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${EXTERNAL_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}glslang$<$<CONFIG:Debug>:_d>${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${EXTERNAL_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}OGLCompiler$<$<CONFIG:Debug>:_d>${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${EXTERNAL_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}OSDependent$<$<CONFIG:Debug>:_d>${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LIBS ${LIBS} glslang)

# googletest
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(googletest EXCLUDE_FROM_ALL)

# imgui
add_library(imgui EXCLUDE_FROM_ALL
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(LIBS ${LIBS} imgui)

# json
add_library(json INTERFACE)
target_include_directories(json INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/json/src)
set(LIBS ${LIBS} json)

# optional-lite
add_library(optional-lite INTERFACE)
target_include_directories(optional-lite INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/optional-lite/include)
set(LIBS ${LIBS} optional-lite)

# sol2
add_library(sol INTERFACE)
target_include_directories(sol INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/sol2/single/sol)
set(LIBS ${LIBS} sol)

# SPIRV-Cross
ExternalProject_Add(SPIRVCross-external
    PREFIX ${EXTERNAL_PREFIX}
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/SPIRV-Cross
    INSTALL_DIR ${EXTERNAL_INSTALL}
    DOWNLOAD_COMMAND ""
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
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/stb)
set(LIBS ${LIBS} stb)

# tinyformat
add_library(tinyformat INTERFACE)
target_include_directories(tinyformat INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/tinyformat)
set(LIBS ${LIBS} tinyformat)

# variant-lite
add_library(variant INTERFACE)
target_include_directories(variant INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/variant/include)
set(LIBS ${LIBS} variant)

# yojimbo_mbedtls
ExternalProject_Add(yojimbo_mbedtls-external
    PREFIX ${EXTERNAL_PREFIX}
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/yojimbo_mbedtls
    INSTALL_DIR ${EXTERNAL_INSTALL}
    DOWNLOAD_COMMAND ""
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

# yojimbo_sodium
if (MSVC)
    ExternalProject_Add(yojimbo_sodium-external
        PREFIX ${EXTERNAL_PREFIX}
        URL https://download.libsodium.org/libsodium/releases/libsodium-1.0.15-msvc.zip
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        UPDATE_COMMAND ""
    )
else()
    ExternalProject_Add(yojimbo_sodium-external
        PREFIX ${EXTERNAL_PREFIX}
        URL ${CMAKE_CURRENT_SOURCE_DIR}/yojimbo_sodium
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ./configure --prefix=${EXTERNAL_INSTALL} --enable-shared=no
        BUILD_COMMAND make
        INSTALL_COMMAND make install
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
        ${EXTERNAL_PREFIX}/src/yojimbo_sodium-external/${SODIUM_ARCH}/$<CONFIG>/${SODIUM_MSVC}/static/libsodium.lib)
else()
    file(MAKE_DIRECTORY ${EXTERNAL_INSTALL}/include)
    set_property(TARGET yojimbo_sodium PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${EXTERNAL_INSTALL}/include)
    set_property(TARGET yojimbo_sodium PROPERTY INTERFACE_LINK_LIBRARIES ${EXTERNAL_INSTALL}/lib/libsodium.a)
endif()

# yojimbo
add_library(yojimbo EXCLUDE_FROM_ALL
    yojimbo/yojimbo.cpp
    yojimbo/tlsf/tlsf.c
    yojimbo/netcode.io/netcode.c
    yojimbo/reliable.io/reliable.c)
target_include_directories(yojimbo PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/yojimbo)
target_include_directories(yojimbo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/yojimbo/netcode.io
    ${CMAKE_CURRENT_SOURCE_DIR}/yojimbo/reliable.io)
target_link_libraries(yojimbo yojimbo_mbedtls yojimbo_sodium)
set(LIBS ${LIBS} yojimbo)

##############################################################
# Push dependencies to the parent scope
##############################################################

set(INCLUDE_DIRS "${INCLUDE_DIRS}" PARENT_SCOPE)
set(LIBS "${LIBS}" PARENT_SCOPE)
