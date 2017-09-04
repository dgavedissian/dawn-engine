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
    -DCMAKE_BUILD_TYPE=$<CONFIG>
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

# Note that "set_property(TARGET Assimp PROPERTY INTERFACE_INCLUDE_DIRECTORIES ...)" checks for the
# existence of the path. Therefore, we create the path beforehand.

# Assimp
ExternalProject_Add(Assimp_Build
    PREFIX ${EXTERNAL_PREFIX}
    INSTALL_DIR ${EXTERNAL_INSTALL}
    URL https://github.com/assimp/assimp/archive/6b4e3177e836dfb05c7eb5e3347b15a9916eebe7.zip
    URL_MD5 f7bb45ebbccdde3997be22e61fdb242b
    BUILD_COMMAND ""
    UPDATE_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install --config $<CONFIG>
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DASSIMP_BUILD_ASSIMP_TOOLS=0
        -DASSIMP_BUILD_TESTS=0
        -DASSIMP_BUILD_ZLIB=1
        -DASSIMP_INSTALL_PDB=0
)
set_target_properties(Assimp_Build PROPERTIES EXCLUDE_FROM_ALL TRUE)
add_library(Assimp STATIC IMPORTED GLOBAL)
add_dependencies(Assimp Assimp_Build)
file(MAKE_DIRECTORY ${EXTERNAL_INSTALL}/include)
set_property(TARGET Assimp PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${EXTERNAL_INSTALL}/include)
if(WIN32)
    set_property(TARGET Assimp PROPERTY INTERFACE_LINK_LIBRARIES
        $<$<CONFIG:Debug>:${EXTERNAL_PREFIX}/src/Assimp_Build-build/contrib/irrXML/Debug/irrXMLd.lib>
        $<$<CONFIG:RelWithDebInfo>:${EXTERNAL_PREFIX}/src/Assimp_Build-build/contrib/irrXML/RelWithDebInfo/irrXML.lib>
        $<$<CONFIG:Release>:${EXTERNAL_PREFIX}/src/Assimp_Build-build/contrib/irrXML/Release/irrXML.lib>
        $<$<CONFIG:Debug>:${EXTERNAL_PREFIX}/src/Assimp_Build-build/contrib/zlib/Debug/zlibstaticd.lib>
        $<$<CONFIG:RelWithDebInfo>:${EXTERNAL_PREFIX}/src/Assimp_Build-build/contrib/zlib/RelWithDebInfo/zlibstatic.lib>
        $<$<CONFIG:Release>:${EXTERNAL_PREFIX}/src/Assimp_Build-build/contrib/zlib/Release/zlibstatic.lib>)
    set_property(TARGET Assimp PROPERTY IMPORTED_LOCATION_DEBUG ${EXTERNAL_INSTALL}/lib/assimp-vc140-mtd.lib)
    set_property(TARGET Assimp PROPERTY IMPORTED_LOCATION_RELWITHDEBINFO ${EXTERNAL_INSTALL}/lib/assimp-vc140-mt.lib)
    set_property(TARGET Assimp PROPERTY IMPORTED_LOCATION_RELEASE ${EXTERNAL_INSTALL}/lib/assimp-vc140-mt.lib)
else()
    set_property(TARGET Assimp PROPERTY INTERFACE_LINK_LIBRARIES
        $<$<CONFIG:Debug>:${EXTERNAL_PREFIX}/src/Assimp_Build-build/contrib/irrXML/libIrrXMLd.a>
        $<$<CONFIG:RelWithDebInfo>:${EXTERNAL_PREFIX}/src/Assimp_Build-build/contrib/irrXML/libIrrXML.a>
        $<$<CONFIG:Release>:${EXTERNAL_PREFIX}/src/Assimp_Build-build/contrib/irrXML/libIrrXML.a>
        $<$<CONFIG:Debug>:${EXTERNAL_PREFIX}/src/Assimp_Build-build/contrib/zlib/libzlibstaticd.a>
        $<$<CONFIG:RelWithDebInfo>:${EXTERNAL_PREFIX}/src/Assimp_Build-build/contrib/zlib/libzlibstatic.a>
        $<$<CONFIG:Release>:${EXTERNAL_PREFIX}/src/Assimp_Build-build/contrib/zlib/libzlibstatic.a>)
    set_property(TARGET Assimp PROPERTY IMPORTED_LOCATION_DEBUG ${EXTERNAL_INSTALL}/lib/libassimpd.a)
    set_property(TARGET Assimp PROPERTY IMPORTED_LOCATION_RELWITHDEBINFO ${EXTERNAL_INSTALL}/lib/libassimp.a)
    set_property(TARGET Assimp PROPERTY IMPORTED_LOCATION_RELEASE ${EXTERNAL_INSTALL}/lib/libassimp.a)
endif()
set(LIBS ${LIBS} Assimp)

# Bullet
ExternalProject_Add(Bullet_Build
    PREFIX ${EXTERNAL_PREFIX}
    INSTALL_DIR ${EXTERNAL_INSTALL}
    URL https://github.com/bulletphysics/bullet3/archive/2.86.zip
    URL_MD5 8b57278946186e9792824069637bbc97
    BUILD_COMMAND ""
    UPDATE_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install --config $<CONFIG>
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DBUILD_EXTRAS=0
        -DBUILD_BULLET2_DEMOS=0
        -DBUILD_BULLET3=0
        -DBUILD_UNIT_TESTS=0
        -DINSTALL_LIBS=1
        -DUSE_MSVC_RUNTIME_LIBRARY_DLL=1
)
set_target_properties(Bullet_Build PROPERTIES EXCLUDE_FROM_ALL TRUE)
add_library(Bullet STATIC IMPORTED GLOBAL)
add_dependencies(Bullet Bullet_Build)
file(MAKE_DIRECTORY ${EXTERNAL_INSTALL}/include/bullet)
set_property(TARGET Bullet PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${EXTERNAL_INSTALL}/include/bullet)
if(WIN32)
    set_property(TARGET Bullet PROPERTY INTERFACE_LINK_LIBRARIES
        $<$<CONFIG:Debug>:${EXTERNAL_INSTALL}/lib/BulletCollision_Debug.lib>
        $<$<CONFIG:RelWithDebInfo>:${EXTERNAL_INSTALL}/lib/BulletCollision.lib>
        $<$<CONFIG:Release>:${EXTERNAL_INSTALL}/lib/BulletCollision.lib>
        $<$<CONFIG:Debug>:${EXTERNAL_INSTALL}/lib/LinearMath_Debug.lib>
        $<$<CONFIG:RelWithDebInfo>:${EXTERNAL_INSTALL}/lib/LinearMath.lib>
        $<$<CONFIG:Release>:${EXTERNAL_INSTALL}/lib/LinearMath.lib>)
    set_property(TARGET Bullet PROPERTY IMPORTED_LOCATION_DEBUG ${EXTERNAL_INSTALL}/lib/BulletDynamics_Debug.lib)
    set_property(TARGET Bullet PROPERTY IMPORTED_LOCATION_RELWITHDEBINFO ${EXTERNAL_INSTALL}/lib/BulletDynamics.lib)
    set_property(TARGET Bullet PROPERTY IMPORTED_LOCATION_RELEASE ${EXTERNAL_INSTALL}/lib/BulletDynamics.lib)
else()
    set_property(TARGET Bullet PROPERTY INTERFACE_LINK_LIBRARIES
        ${EXTERNAL_INSTALL}/lib/libBulletDynamics.a
        ${EXTERNAL_INSTALL}/lib/libBulletCollision.a
        ${EXTERNAL_INSTALL}/lib/libLinearMath.a)
    set_property(TARGET Bullet PROPERTY IMPORTED_LOCATION ${EXTERNAL_INSTALL}/lib/libBulletDynamics.a)
endif()
set(LIBS ${LIBS} Bullet)

# MathGeoLib
ExternalProject_Add(MathGeoLib_Build
    PREFIX ${EXTERNAL_PREFIX}
    INSTALL_DIR ${EXTERNAL_INSTALL}
    URL https://github.com/davedissian/dawnengine-MathGeoLib/archive/master.zip
    URL_MD5 d1fd2ed6aa2284049234f377c6a6f1e9
    BUILD_COMMAND ""
    UPDATE_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install --config $<CONFIG>
    CMAKE_ARGS ${EXTERNAL_ARGS}
)
set_target_properties(MathGeoLib_Build PROPERTIES EXCLUDE_FROM_ALL TRUE)
add_library(MathGeoLib STATIC IMPORTED GLOBAL)
add_dependencies(MathGeoLib MathGeoLib_Build)
file(MAKE_DIRECTORY ${EXTERNAL_INSTALL}/include/MathGeoLib)
set_property(TARGET MathGeoLib PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${EXTERNAL_INSTALL}/include/MathGeoLib)
if(WIN32)
    set_property(TARGET MathGeoLib PROPERTY IMPORTED_LOCATION_DEBUG ${EXTERNAL_INSTALL}/lib/MathGeoLib_d.lib)
    set_property(TARGET MathGeoLib PROPERTY IMPORTED_LOCATION_RELWITHDEBINFO ${EXTERNAL_INSTALL}/lib/MathGeoLib.lib)
    set_property(TARGET MathGeoLib PROPERTY IMPORTED_LOCATION_RELEASE ${EXTERNAL_INSTALL}/lib/MathGeoLib.lib)
else()
    set_property(TARGET MathGeoLib PROPERTY IMPORTED_LOCATION_DEBUG ${EXTERNAL_INSTALL}/lib/libMathGeoLib_d.a)
    set_property(TARGET MathGeoLib PROPERTY IMPORTED_LOCATION_RELWITHDEBINFO ${EXTERNAL_INSTALL}/lib/libMathGeoLib.a)
    set_property(TARGET MathGeoLib PROPERTY IMPORTED_LOCATION_RELEASE ${EXTERNAL_INSTALL}/lib/libMathGeoLib.alib)
endif()
set(LIBS ${LIBS} MathGeoLib)

# Luajit
if (MSVC)
    ExternalProject_Add(Luajit_Build
        PREFIX ${EXTERNAL_PREFIX}
        URL http://luajit.org/download/LuaJIT-2.1.0-beta2.zip
        URL_MD5 b5d943c0174ca217736e2ddc2d9721c3
        BINARY_DIR ${EXTERNAL_PREFIX}/src/Luajit_Build/src
        CONFIGURE_COMMAND ""
        BUILD_COMMAND msvcbuild.bat static
        INSTALL_COMMAND ""
        UPDATE_COMMAND ""
    )
else()
    ExternalProject_Add(Luajit_Build
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
set_target_properties(Luajit_Build PROPERTIES EXCLUDE_FROM_ALL TRUE)
add_library(Luajit STATIC IMPORTED GLOBAL)
add_dependencies(Luajit Luajit_Build)
file(MAKE_DIRECTORY ${EXTERNAL_PREFIX}/src/Luajit_Build/src)
set_property(TARGET Luajit PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${EXTERNAL_PREFIX}/src/Luajit_Build/src)
if(WIN32)
    set_property(TARGET Luajit PROPERTY IMPORTED_LOCATION ${EXTERNAL_PREFIX}/src/Luajit_Build/src/lua51.lib)
else()
    set_property(TARGET Luajit PROPERTY IMPORTED_LOCATION ${EXTERNAL_INSTALL}/lib/libluajit-5.1.a)
endif()
set(LIBS ${LIBS} Luajit)


##############################################################
# Packaged Dependencies
##############################################################

# Build static libraries as PIC
if(UNIX) # TODO: If is Clang or GCC
    add_definitions(-fPIC)
endif()

# gl3w
add_library(gl3w EXCLUDE_FROM_ALL gl3w/gl3w.c gl3w/GL/gl3w.h gl3w/GL/glcorearb.h)
target_include_directories(gl3w PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/gl3w)
set(LIBS ${LIBS} gl3w)

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
set(GLFW_USE_CHDIR OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw EXCLUDE_FROM_ALL)
target_include_directories(glfw PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include)
set(LIBS ${LIBS} glfw)

# glslang
set(SKIP_GLSLANG_INSTALL ON CACHE BOOL "" FORCE)
add_subdirectory(glslang EXCLUDE_FROM_ALL)
target_include_directories(glslang PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glslang/glslang/Public ${CMAKE_CURRENT_SOURCE_DIR}/glslang/SPIRV)
set(LIBS ${LIBS} glslang SPIRV)

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

# SPIRV-Cross
add_subdirectory(SPIRV-Cross EXCLUDE_FROM_ALL)
#target_include_directories(glfw PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include)
set(LIBS ${LIBS} spirv-cross-glsl spirv-cross-hlsl spirv-cross-msl)

# stb_image
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/stb_image)
set(LIBS ${LIBS} stb_image)

# tinyformat
add_library(tinyformat INTERFACE)
target_include_directories(tinyformat INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/tinyformat)
set(LIBS ${LIBS} tinyformat)

# variant
add_library(variant INTERFACE)
target_include_directories(variant INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/variant/include)
set(LIBS ${LIBS} variant)


##############################################################
# Push dependencies to the parent scope
##############################################################

set(INCLUDE_DIRS "${INCLUDE_DIRS}" PARENT_SCOPE)
set(LIBS "${LIBS}" PARENT_SCOPE)
