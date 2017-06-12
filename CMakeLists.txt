# Dawn Engine
# Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
cmake_minimum_required(VERSION 3.0)
project(Dawn)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_MACOSX_RPATH 1)


###############################################
# Setup
###############################################

# Include modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Limit configuration types
set(CMAKE_CONFIGURATION_TYPES Release RelWithDebInfo Debug)


###############################################
# Utilities
###############################################

include(Utilities)


###############################################
# Platform specific stuff
###############################################

# Fix for "error C3859: virtual memory range for PCH exceeded" with MSVC
if(MSVC)
    string(REGEX REPLACE "/Zm[0-9]+ " "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm500" CACHE STRING "" FORCE)
endif()

###############################################
# Helpful functions
###############################################

function(add_game GAME_NAME SOURCE_FILES SHADER_SOURCE_FILES)
    # Executable type
    if(WIN32)
        set(TYPE WIN32)
    elseif(APPLE)
        set(TYPE MACOSX_BUNDLE)
    endif()

    # Add executable
    add_executable(${GAME_NAME} ${TYPE} ${SOURCE_FILES})

    # Compile in C++14 mode
    enable_cpp14(${GAME_NAME})
    enable_maximum_warnings(${GAME_NAME})

    # Add include directories
    get_target_property(DAWN_INCLUDE_DIRS Dawn INCLUDE_DIRECTORIES)
    target_include_directories(${GAME_NAME} PRIVATE
        ${DAWN_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR})

    # Add libraries
    target_link_libraries(${GAME_NAME} Dawn)
    set_output_dir(${GAME_NAME} ${CMAKE_SOURCE_DIR}/bin)
    set_target_properties(${GAME_NAME} PROPERTIES DEBUG_POSTFIX _d)

    # Compile shaders
    set(COMPILED_SHADERS "")
    set(SHADER_PATH "${CMAKE_SOURCE_DIR}/bin/compiled_shaders")
    foreach(SHADER ${SHADER_SOURCE_FILES})
        set(SHADER_OUT "${SHADER_PATH}/${SHADER}")
        add_custom_command(
            OUTPUT "${SHADER_OUT}"
            COMMAND "$<TARGET_FILE:shaderc>" -o "${LUA_COMPILED_SCRIPTS}/${F}"
            COMMENT "Compiling shader ${SHADER} to binary")
        list (APPEND LUA_COMPILED_FILES "${SHADER_OUT}")
    endforeach()
    add_custom_target(${GAME_NAME}_Shaders ${COMPILED_SHADERS})
    add_dependencies(${GAME_NAME} ${GAME_NAME}_Shaders)

    # Currently, Dawn can only be linked dynamically
    #set_target_properties(${GAME_NAME} PROPERTIES COMPILE_FLAGS "-DDW_DLL")

    # LuaJIT on 64-bit OS X requires these linker flags for executables
    if(APPLE)
        set_target_properties(${GAME_NAME} PROPERTIES LINK_FLAGS "-pagezero_size 10000 -image_base 100000000")
    endif()

    # Copy over shared library dependencies
    get_target_property(LIB_DIR Dawn LIBRARY_OUTPUT_DIRECTORY)
    if(APPLE)
        #set(BUNDLE_DIR ${CMAKE_SOURCE_DIR}/bin/${GAME_NAME}.app)
        #set(FRAMEWORKS_DIR ${BUNDLE_DIR}/Contents/Frameworks)
        #add_custom_command(TARGET ${GAME_NAME} POST_BUILD
        #    COMMAND mkdir -p ${FRAMEWORKS_DIR}
        #    COMMAND cp -r ${LIB_DIR}/Dawn.framework ${FRAMEWORKS_DIR}/Dawn.framework)
    else()
        #add_custom_command(TARGET ${GAME_NAME} POST_BUILD COMMAND
        #    ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Dawn> $<TARGET_FILE_DIR:${GAME_NAME}>)
    endif()
endfunction()


###############################################
# Source
###############################################

add_subdirectory(src)


###############################################
# Tests
###############################################

add_subdirectory(test)


###############################################
# Installation
###############################################
# TODO

