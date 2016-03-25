# Dawn Engine
# Written by David Avedissian (c) 2012-2016 (git@davedissian.com)

# Find SDL2 on the system
find_package(SDL2 REQUIRED)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${SDL2_INCLUDE_DIR})
set(LIBS ${LIBS} ${SDL2_LIBRARIES})

# Add bundled external libraries
set(INCLUDE_DIRS ${INCLUDE_DIRS}
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

set(EXTERNAL_DEPS Bullet Ogre MathGeoLib Luajit LibRocket RakNet IrrKlang PARENT_SCOPE)


##############################################################
# Packaged Dependencies
##############################################################

# Build static libraries as PIC
if(UNIX) # TODO: If is Clang or GCC
    add_definitions(-fPIC)
endif()

# ImGui
add_library(ImGui ImGui/imgui.cpp)
target_include_directories(ImGui PRIVATE ${INCLUDE_DIRS})
set_target_properties(ImGui PROPERTIES FOLDER "3rdparty")
set(LIBS ${LIBS} ImGui)

# YamlCpp
file(GLOB YAML_CPP_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} YamlCpp/src/*.cpp)
add_library(YamlCpp ${YAML_CPP_SRC})
target_include_directories(YamlCpp PRIVATE ${INCLUDE_DIRS})
set_target_properties(YamlCpp PROPERTIES FOLDER "3rdparty")
set(LIBS ${LIBS} YamlCpp)


##############################################################
# Push dependencies to the parent scope
##############################################################

set(INCLUDE_DIRS "${INCLUDE_DIRS}" PARENT_SCOPE)
set(LIBS "${LIBS}" PARENT_SCOPE)
