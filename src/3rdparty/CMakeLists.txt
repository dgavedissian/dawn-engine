# Dawn Engine
# Written by David Avedissian (c) 2012-2016 (git@davedissian.com)

##############################################################
# DISCLAIMER
##############################################################
# This entire file is basically a gigantic hack, because C++ has no concept of dependency resolution
# and modules WHATSOEVER. You've been warned...
##############################################################

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

# OGRE
if(APPLE)
    set(OGREDEPS_PATCH_COMMAND
        sed -ie "s/set\(CMAKE_OSX_DEPLOYMENT_TARGET/#set\(CMAKE_OSX_DEPLOYMENT_TARGET/g" CMakeLists.txt)
endif()
if(WIN32)
    set(FREETYPE_LIB Release/freetype.lib)
else()
    set(FREETYPE_LIB libfreetype.a)
endif()
ExternalProject_Add(OgreDeps
    PREFIX ${EXTERNAL_PREFIX}
    INSTALL_DIR ${EXTERNAL_INSTALL}
    HG_REPOSITORY https://bitbucket.org/cabalistic/ogredeps
    PATCH_COMMAND ${OGREDEPS_PATCH_COMMAND}
    UPDATE_COMMAND ""
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DOGREDEPS_BUILD_CG=0
        -DOGREDEPS_BUILD_AMD_QBS=0
        -DOGREDEPS_BUILD_NVIDIA_NVAPI=0
        -DOGREDEPS_BUILD_SDL2=0
        -DOGREDEPS_BUILD_OIS=0)
set_target_properties(OgreDeps PROPERTIES FOLDER "3rdparty")

ExternalProject_Add(Ogre
    DEPENDS OgreDeps
    PREFIX ${EXTERNAL_PREFIX}
    INSTALL_DIR ${EXTERNAL_INSTALL}
    HG_REPOSITORY https://bitbucket.org/sinbad/ogre
    HG_TAG 374723e
    UPDATE_COMMAND ""
    CMAKE_ARGS ${EXTERNAL_ARGS}
        -DOIS_INCLUDE_DIR=${EXTERNAL_PREFIX}/install # dummy directory to shut CMake up
        -DOGRE_DEPENDENCIES_DIR=${EXTERNAL_INSTALL}
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
set_target_properties(Ogre PROPERTIES FOLDER "3rdparty")

set(OGRE_INCLUDE_DIRS
    ${EXTERNAL_INSTALL}/include/OGRE
    ${EXTERNAL_INSTALL}/include/OGRE/RenderSystems/GL3Plus/include)
if(WIN32)
    set(OGRE_LIBRARIES
        debug ${EXTERNAL_INSTALL}/lib/Debug/FreeImage_d.lib
        optimized ${EXTERNAL_INSTALL}/lib/Release/FreeImage.lib
        debug ${EXTERNAL_INSTALL}/lib/Debug/freetype_d.lib
        optimized ${EXTERNAL_INSTALL}/lib/Release/freetype.lib
        debug ${EXTERNAL_INSTALL}/lib/Debug/zlib_d.lib
        optimized ${EXTERNAL_INSTALL}/lib/Release/zlib.lib
        debug ${EXTERNAL_INSTALL}/lib/Debug/zziplib_d.lib
        optimized ${EXTERNAL_INSTALL}/lib/Release/zziplib.lib
        debug ${EXTERNAL_INSTALL}/lib/Debug/OgreMainStatic_d.lib
        optimized ${EXTERNAL_INSTALL}/lib/Release/OgreMainStatic.lib
        debug ${EXTERNAL_INSTALL}/lib/Debug/opt/RenderSystem_GL3PlusStatic_d.lib
        optimized ${EXTERNAL_INSTALL}/lib/Release/opt/RenderSystem_GL3PlusStatic.lib)
elseif(APPLE)
    set(OGRE_LIBRARIES
        ${EXTERNAL_INSTALL}/lib/libFreeImage.a
        ${EXTERNAL_INSTALL}/lib/libfreetype.a
        ${EXTERNAL_INSTALL}/lib/libzlib.a
        ${EXTERNAL_INSTALL}/lib/libzziplib.a
        ${EXTERNAL_INSTALL}/lib/libOgreMainStatic.a
        ${EXTERNAL_INSTALL}/lib/libRenderSystem_GL3PlusStatic.a)
else()
    set(OGRE_LIBRARIES
        debug ${EXTERNAL_INSTALL}/lib/libFreeImage_d.a
        optimized ${EXTERNAL_INSTALL}/lib/libFreeImage.a
        debug ${EXTERNAL_INSTALL}/lib/libfreetype_d.a
        optimized ${EXTERNAL_INSTALL}/lib/libfreetype.a
        debug ${EXTERNAL_INSTALL}/lib/libzlib_d.a
        optimized ${EXTERNAL_INSTALL}/lib/libzlib.a
        debug ${EXTERNAL_INSTALL}/lib/libzziplib_d.a
        optimized ${EXTERNAL_INSTALL}/lib/libzziplib.a
        debug ${EXTERNAL_INSTALL}/lib/libOgreMainStatic_d.a
        optimized ${EXTERNAL_INSTALL}/lib/libOgreMainStatic.a
        debug ${EXTERNAL_INSTALL}/lib/OGRE/libRenderSystem_GL3PlusStatic_d.a
        optimized ${EXTERNAL_INSTALL}/lib/OGRE/libRenderSystem_GL3PlusStatic.a)
endif()
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${OGRE_INCLUDE_DIRS})
set(LIBS ${LIBS} ${OGRE_LIBRARIES})

# Ogre External Dependencies
find_package(OpenGL REQUIRED)
if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    find_library(XAW_LIBRARY NAMES Xaw)
    set(LIBS ${LIBS} ${X11_LIBRARIES} ${X11_Xt_LIBRARIES} ${XAW_LIBRARY} ${X11_Xrandr_LIB})
endif()
set(LIBS ${LIBS} ${OPENGL_LIBRARY})

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
        -DPU_BUILD_TOOLS=0
        -DFREETYPE_LIBRARY=${EXTERNAL_INSTALL}/lib/${FREETYPE_LIB}) # Not sure why this is needed..
set_target_properties(ParticleUniverse PROPERTIES FOLDER "3rdparty")
if(WIN32)
    set(LIBS ${LIBS}
        debug ${EXTERNAL_INSTALL}/lib/Plugin_ParticleUniverse_d.lib
        optimized ${EXTERNAL_INSTALL}/lib/Plugin_ParticleUniverse.lib)
else()
    set(LIBS ${LIBS}
        debug ${EXTERNAL_INSTALL}/lib/Plugin_ParticleUniverse_d.a
        optimized ${EXTERNAL_INSTALL}/lib/Plugin_ParticleUniverse.a)
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

# libRocket
ExternalProject_Add(LibRocket
    DEPENDS OgreDeps
    PREFIX ${EXTERNAL_PREFIX}
    INSTALL_DIR ${EXTERNAL_INSTALL}
    GIT_REPOSITORY https://github.com/davedissian/dawnengine-libRocket.git
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ${CMAKE_COMMAND} ../LibRocket/Build
        ${EXTERNAL_ARGS}
        -DBUILD_SHARED_LIBS=0
        -DFREETYPE_INCLUDE_DIR_ft2build=${EXTERNAL_INSTALL}/include
        -DFREETYPE_INCLUDE_DIR_freetype2=${EXTERNAL_INSTALL}/include/freetype
        -DFREETYPE_LIBRARY=${EXTERNAL_INSTALL}/lib/${FREETYPE_LIB})
set_target_properties(LibRocket PROPERTIES FOLDER "3rdparty")
if(WIN32)
    set(LIBS ${LIBS}
        debug ${EXTERNAL_INSTALL}/lib/RocketCore_d.lib
        optimized ${EXTERNAL_INSTALL}/lib/RocketCore.lib
        debug ${EXTERNAL_INSTALL}/lib/RocketControls_d.lib
        optimized ${EXTERNAL_INSTALL}/lib/RocketControls.lib
        debug ${EXTERNAL_INSTALL}/lib/RocketDebugger_d.lib
        optimized ${EXTERNAL_INSTALL}/lib/RocketDebugger.lib)
else()
    set(LIBS ${LIBS}
        debug ${EXTERNAL_INSTALL}/lib/libRocketCore_d.a
        optimized ${EXTERNAL_INSTALL}/lib/libRocketCore.a
        debug ${EXTERNAL_INSTALL}/lib/libRocketControls_d.a
        optimized ${EXTERNAL_INSTALL}/lib/libRocketControls.a
        debug ${EXTERNAL_INSTALL}/lib/libRocketDebugger_d.a
        optimized ${EXTERNAL_INSTALL}/lib/libRocketDebugger.a)
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
set_target_properties(RakNet PROPERTIES FOLDER "3rdparty")
if(WIN32)
    set(LIBS ${LIBS}
        debug ${EXTERNAL_INSTALL}/lib/RakNetLibStatic_d.lib
        optimized ${EXTERNAL_INSTALL}/lib/RakNetLibStatic.lib)
else()
    set(LIBS ${LIBS}
        debug ${EXTERNAL_INSTALL}/lib/libRakNetLibStatic_d.a
        optimized ${EXTERNAL_INSTALL}/lib/libRakNetLibStatic.a)
endif()

# IrrKlang
if(WIN32)
    ExternalProject_Add(IrrKlang
        PREFIX ${EXTERNAL_PREFIX}
        URL http://www.ambiera.at/downloads/irrKlang-32bit-1.5.0.zip
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/irrKlang-1.5.0/bin/win32-visualStudio/irrKlang.dll ${EXTERNAL_INSTALL}/lib/
        COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/irrKlang-1.5.0/lib/Win32-visualStudio/irrKlang.lib ${EXTERNAL_INSTALL}/lib/
        COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/irrKlang-1.5.0/include ${EXTERNAL_INSTALL}/include)
    set(LIBS ${LIBS}
        ${EXTERNAL_INSTALL}/lib/irrKlang.lib)
else()
    if(APPLE)
        set(IRRKLANG_LIB_PREFIX bin/macosx-gcc)
        set(IRRKLANG_LIBIRRKLANG libirrklang.dylib)
        set(IRRKLANG_PATCH_COMMAND install_name_tool -id @rpath/libirrklang.dylib <SOURCE_DIR>/bin/macosx-gcc/libirrklang.dylib)
    else()
        set(IRRKLANG_LIB_PREFIX bin/linux-gcc-64)
        set(IRRKLANG_LIBIRRKLANG libIrrKlang.so)
    endif()
    ExternalProject_Add(IrrKlang
        PREFIX ${EXTERNAL_PREFIX}
        URL http://www.ambiera.at/downloads/irrKlang-64bit-1.5.0.zip
        PATCH_COMMAND ${IRRKLANG_PATCH_COMMAND}
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        COMMAND ${IRRKLANG_WINDOWS_COPY_IMPORT_LIB}
        COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/${IRRKLANG_LIB_PREFIX}/${IRRKLANG_LIBIRRKLANG} ${EXTERNAL_INSTALL}/lib/
        COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/include ${EXTERNAL_INSTALL}/include)
    set(LIBS ${LIBS}
        ${EXTERNAL_INSTALL}/lib/${IRRKLANG_LIBIRRKLANG})
endif()
set_target_properties(IrrKlang PROPERTIES FOLDER "3rdparty")

set(EXTERNAL_DEPS Bullet Ogre ParticleUniverse MathGeoLib Luajit LibRocket RakNet IrrKlang PARENT_SCOPE)


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
