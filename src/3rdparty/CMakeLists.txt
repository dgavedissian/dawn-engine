# Dawn Engine
# Copyright (c) 2012-2015 David Avedissian (git@davedissian.com)

# Find SDL2 on the system
find_package(SDL2 REQUIRED)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${SDL2_INCLUDE_DIR})
set(LIBS ${LIBS} ${SDL2_LIBRARIES})

# Add bundled external libraries
set(INCLUDE_DIRS ${INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/BtOgre
    ${CMAKE_CURRENT_SOURCE_DIR}/FastDelegate
    ${CMAKE_CURRENT_SOURCE_DIR}/ImGui
    ${CMAKE_CURRENT_SOURCE_DIR}/LuaBridge
    ${CMAKE_CURRENT_SOURCE_DIR}/YamlCpp/include)


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
    -DCMAKE_C_FLAGS=-fPIC
    -DCMAKE_CXX_FLAGS=-fPIC)

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
        -DINSTALL_LIBS=1)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${EXTERNAL_INSTALL}/include/bullet)
set(LIBS ${LIBS}
    ${EXTERNAL_INSTALL}/lib/libBulletCollision.a
    ${EXTERNAL_INSTALL}/lib/libBulletDynamics.a
    ${EXTERNAL_INSTALL}/lib/libLinearMath.a)

# OGRE
ExternalProject_Add(OgreDeps
    PREFIX ${EXTERNAL_PREFIX}
    INSTALL_DIR ${CMAKE_BINARY_DIR}/dependencies/ogredeps/ogredeps
    HG_REPOSITORY https://bitbucket.org/cabalistic/ogredeps
    UPDATE_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${EXTERNAL_PREFIX}/install
        -DOGREDEPS_BUILD_OIS=0)

ExternalProject_Add(Ogre
    DEPENDS OgreDeps
    PREFIX ${EXTERNAL_PREFIX}
    INSTALL_DIR ${EXTERNAL_INSTALL}
    HG_REPOSITORY https://bitbucket.org/sinbad/ogre
    HG_TAG 374723e
    UPDATE_COMMAND ""
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DOIS_INCLUDE_DIR="" # this is needed for some reason?
        -DOGRE_DEPENDENCIES_DIR=${EXTERNAL_PREFIX}/install
        -DOGRE_BUILD_COMPONENT_MESHLODGENERATOR=0
        -DOGRE_BUILD_COMPONENT_OVERLAY=0
        -DOGRE_BUILD_COMPONENT_PAGING=0
        -DOGRE_BUILD_COMPONENT_PROPERTY=0
        -DOGRE_BUILD_COMPONENT_RTSHADERSYSTEM=0
        -DOGRE_BUILD_COMPONENT_TERRAIN=0
        -DOGRE_BUILD_COMPONENT_VOLUME=0
        -DOGRE_BUILD_PLUGIN_BSP=0
        -DOGRE_BUILD_PLUGIN_CG=0
        -DOGRE_BUILD_PLUGIN_OCTREE=0
        -DOGRE_BUILD_PLUGIN_PCZ=0
        -DOGRE_BUILD_PLUGIN_PFX=0
        -DOGRE_BUILD_RENDERSYSTEM_GL=0
        -DOGRE_BUILD_RENDERSYSTEM_GL3PLUS=1
        -DOGRE_BUILD_SAMPLES=0
        -DOGRE_BUILD_TOOLS=0
        -DOGRE_CONFIG_THREADS=0
        -DOGRE_INSTALL_DOCS=0
        -DOGRE_INSTALL_PDB=0
        -DOGRE_INSTALL_SAMPLES=0
        -DOGRE_STATIC=1
        -DOGRE_UNITY_BUILD=1
        -DOGRE_USE_BOOST=0)

set(OGRE_INCLUDE_DIRS ${EXTERNAL_INSTALL}/include/OGRE)
if (WIN32)
else()
    set(OGRE_LIBRARIES
        ${EXTERNAL_PREFIX}/install/lib/libFreeImage.a
        ${EXTERNAL_PREFIX}/install/lib/libfreetype.a
        ${EXTERNAL_PREFIX}/install/lib/libzlib.a
        ${EXTERNAL_PREFIX}/install/lib/libzziplib.a
        ${EXTERNAL_INSTALL}/lib/libOgreMainStatic.a
        ${EXTERNAL_INSTALL}/lib/OGRE/libRenderSystem_GL3PlusStatic.a)
endif()
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${OGRE_INCLUDE_DIRS})
set(LIBS ${LIBS} ${OGRE_LIBRARIES})

# Ogre External Dependencies
if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    find_library(XAW_LIBRARY NAMES Xaw)
    set(LIBS ${LIBS} ${X11_LIBRARIES} ${X11_Xt_LIBRARIES} ${XAW_LIBRARY} ${X11_Xrandr_LIB})
endif()

# ParticleUniverse
ExternalProject_Add(ParticleUniverse
    DEPENDS Ogre
    PREFIX ${EXTERNAL_PREFIX}
    INSTALL_DIR ${EXTERNAL_INSTALL}
    GIT_REPOSITORY https://github.com/davedissian/dawnengine-particleuniverse.git
    UPDATE_COMMAND ""
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DBUILD_SHARED_LIBS=0
        -DOGRE_FOUND=1 # supress FindOgre.cmake
        -DOGRE_STATIC=1
        -DOGRE_INCLUDE_DIRS=${OGRE_INCLUDE_DIRS}
        -DOGRE_LIBRARIES=${OGRE_LIBRARIES}
        -DPU_BUILD_SAMPLES=0
        -DPU_BUILD_TOOLS=0)
if(WIN32)
else()
    set(LIBS ${LIBS}
        ${OGRE_LIBRARIES}
        ${EXTERNAL_INSTALL}/lib/Plugin_ParticleUniverse.a)
endif()

# MathGeoLib
ExternalProject_Add(MathGeoLib
    PREFIX ${EXTERNAL_PREFIX}
    INSTALL_DIR ${EXTERNAL_INSTALL}
    GIT_REPOSITORY https://github.com/davedissian/dawnengine-MathGeoLib.git
    UPDATE_COMMAND ""
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DBUILD_SHARED_LIBS=0)
set(INCLUDE_DIRS ${INCLUDE_DIRS}
    ${EXTERNAL_INSTALL}/include/MathGeoLib)
if(WIN32)
else()
    set(LIBS ${LIBS}
        ${EXTERNAL_INSTALL}/lib/libMathGeoLib.a)
endif()

# Luajit
ExternalProject_Add(Lua
    PREFIX ${EXTERNAL_PREFIX}
    INSTALL_DIR ${EXTERNAL_INSTALL}
    GIT_REPOSITORY https://github.com/davedissian/dawnengine-luajit.git
    UPDATE_COMMAND ""
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DBUILD_SHARED_LIBS=0
        -DLUA_BUILD_WLUA=0)
set(LIBS ${LIBS}
    ${EXTERNAL_INSTALL}/lib/liblua.a)

# libRocket
if(WIN32)
    set(FREETYPE_LIB Release/freetype.lib)
else()
    set(FREETYPE_LIB libfreetype.a)
endif()
ExternalProject_Add(LibRocket
    DEPENDS OgreDeps
    PREFIX ${EXTERNAL_PREFIX}
    INSTALL_DIR ${EXTERNAL_INSTALL}
    GIT_REPOSITORY https://github.com/davedissian/dawnengine-libRocket.git
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ${CMAKE_COMMAND} ../LibRocket/Build
        ${EXTERNAL_ARGS}
        -DBUILD_SHARED_LIBS=0
        -DFREETYPE_INCLUDE_DIR_ft2build=${EXTERNAL_PREFIX}/install/include
        -DFREETYPE_INCLUDE_DIR_freetype2=${EXTERNAL_PREFIX}/install/include/freetype
        -DFREETYPE_LIBRARY=${EXTERNAL_PREFIX}/install/lib/${FREETYPE_LIB})
if(WIN32)
else()
    set(LIBS ${LIBS}
        ${EXTERNAL_INSTALL}/lib/libRocketControls.a
        ${EXTERNAL_INSTALL}/lib/libRocketCore.a
        ${EXTERNAL_INSTALL}/lib/libRocketDebugger.a)
endif()

# RakNet
ExternalProject_Add(RakNet
    PREFIX ${EXTERNAL_PREFIX}
    INSTALL_DIR ${EXTERNAL_INSTALL}
    GIT_REPOSITORY https://github.com/davedissian/dawnengine-RakNet.git
    UPDATE_COMMAND ""
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DRAKNET_ENABLE_DLL=0
        -DRAKNET_ENABLE_SAMPLES=0)
if(WIN32)
else()
    set(LIBS ${LIBS}
        ${EXTERNAL_INSTALL}/lib/libRakNetLibStatic.a)
endif()

# IrrKlang
if(WIN32)
elseif(APPLE)
    set(IRRKLANG_LIB_PREFIX bin/macosx-gcc)
    set(IRRKLANG_LIBIRRKLANG libirrklang.dylib)
    set(IRRKLANG_IKPFLAC ikpFLAC.dylib)
    set(IRRKLANG_IKPMP3 ikpMP3.dylib)
else()
    set(IRRKLANG_LIB_PREFIX bin/linux-gcc-64)
    set(IRRKLANG_LIBIRRKLANG libIrrKlang.so)
    set(IRRKLANG_IKPFLAC ikpFlac.so)
    set(IRRKLANG_IKPMP3 ikpMP3.so)
endif()
ExternalProject_Add(IrrKlang
    PREFIX ${EXTERNAL_PREFIX}
    URL http://www.ambiera.at/downloads/irrKlang-64bit-1.5.0.zip
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/${IRRKLANG_LIB_PREFIX}/${IRRKLANG_LIBIRRKLANG} ${EXTERNAL_INSTALL}/lib/
    COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/${IRRKLANG_LIB_PREFIX}/${IRRKLANG_IKPFLAC} ${EXTERNAL_INSTALL}/lib/lib${IRRKLANG_IKPFLAC}
    COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/${IRRKLANG_LIB_PREFIX}/${IRRKLANG_IKPMP3} ${EXTERNAL_INSTALL}/lib/lib${IRRKLANG_IKPMP3}
    COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/include ${EXTERNAL_INSTALL}/include)
if(WIN32)
    set(LIBS ${LIBS}
        ${EXTERNAL_INSTALL}/lib/${IRRKLANG_LIBIRRKLANG}
        ${EXTERNAL_INSTALL}/lib/${IRRKLANG_IKPFLAC}
        ${EXTERNAL_INSTALL}/lib/${IRRKLANG_IKPMP3})
else()
    set(LIBS ${LIBS}
        ${EXTERNAL_INSTALL}/lib/${IRRKLANG_LIBIRRKLANG}
        ${EXTERNAL_INSTALL}/lib/lib${IRRKLANG_IKPFLAC}
        ${EXTERNAL_INSTALL}/lib/lib${IRRKLANG_IKPMP3})
endif()

set(EXTERNAL_DEPS Bullet Ogre ParticleUniverse MathGeoLib Lua LibRocket RakNet IrrKlang PARENT_SCOPE)


##############################################################
# Packaged Dependencies
##############################################################
include_directories(${INCLUDE_DIRS})

# Build static libraries as PIC
if(UNIX) # TODO: If is Clang or GCC
    add_definitions(-fPIC)
endif()

# BtOgre
add_library(BtOgre BtOgre/BtOgre.cpp)
set_target_properties(BtOgre PROPERTIES FOLDER "3rdparty")
set(LIBS ${LIBS} BtOgre)
add_dependencies(BtOgre Bullet Ogre)

# ImGui
add_library(ImGui ImGui/imgui.cpp)
set_target_properties(ImGui PROPERTIES FOLDER "3rdparty")
set(LIBS ${LIBS} ImGui)

# YamlCpp
file(GLOB YAML_CPP_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} YamlCpp/src/*.cpp)
add_library(YamlCpp ${YAML_CPP_SRC})
set_target_properties(YamlCpp PROPERTIES FOLDER "3rdparty")
set(LIBS ${LIBS} YamlCpp)


##############################################################
# Push dependencies to the parent scope
##############################################################

set(INCLUDE_DIRS "${INCLUDE_DIRS}" PARENT_SCOPE)
set(LIBS "${LIBS}" PARENT_SCOPE)
