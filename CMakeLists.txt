# Dawn Engine
# Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
cmake_minimum_required(VERSION 3.11)

set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CFlagOverrides.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CXXFlagOverrides.cmake)

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

if(MSVC)
    # Workaround for 'warning C4577: 'noexcept' used with no exception handling mode specified;
    # termination on exception is not guaranteed. Specify /EHsc'
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")

    # As of MSVC 2017 15.8, we need to set this define to use "correct" alignment behaviour.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_ENABLE_EXTENDED_ALIGNED_STORAGE")
endif(MSVC)

###############################################
# Helpful functions
###############################################

function(add_dawn_executable GAME_NAME)
    cmake_parse_arguments(
        TARGET # prefix of output variables
        "" # list of names of the boolean arguments (only defined ones will be true)
        "" # list of names of mono-valued arguments
        "SRCS;RESOURCE_PKGS" # list of names of multi-valued arguments (output variables are lists)
        ${ARGN} # arguments of the function to parse, here we take the all original ones
    )

    # Append base resource package.
    # TODO(David): Support base dir when executing this script from _outside_ this CMake file.
    set(TARGET_RESOURCE_PKGS ${CMAKE_SOURCE_DIR}/media/base ${TARGET_RESOURCE_PKGS})

    message(STATUS "add_dawn_executable: Adding ${GAME_NAME}.")
    message(STATUS "add_dawn_executable:   Sources: ${TARGET_SRCS}")
    message(STATUS "add_dawn_executable:   Resources: ${TARGET_RESOURCE_PKGS}")

    set(OUT_DIR ${CMAKE_SOURCE_DIR}/bin)

    # Executable type
    if(WIN32)
        set(TYPE WIN32)
    elseif(APPLE)
        set(TYPE MACOSX_BUNDLE)
    endif()

    # Add executable
    add_executable(${GAME_NAME} ${TYPE} ${TARGET_SRCS})
    set_output_dir(${GAME_NAME} ${CMAKE_BINARY_DIR})

    # Compile in C++14 mode
    enable_cpp14(${GAME_NAME})
    enable_maximum_warnings(${GAME_NAME})

    # Add include directories
    get_target_property(DAWN_INCLUDE_DIRS DwEngine INCLUDE_DIRECTORIES)
    target_include_directories(${GAME_NAME} PRIVATE
        ${DAWN_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR})

    # Add libraries
    target_link_libraries(${GAME_NAME} DwEngine)
    set_target_properties(${GAME_NAME} PROPERTIES DEBUG_POSTFIX _d)
    if(EMSCRIPTEN)
        set_target_properties(${GAME_NAME} PROPERTIES SUFFIX .html)
        set(link_flags "-s DEMANGLE_SUPPORT=1 -s TOTAL_MEMORY=256MB -s ALLOW_MEMORY_GROWTH=1 --no-heap-copy")
        if(DW_DISABLE_WASM)
            set(link_flags "${link_flags} -s WASM=0")
        endif()
        # Include resources.
        foreach (resource ${TARGET_RESOURCE_PKGS})
            get_filename_component(resource_name ${resource} NAME)
            set(link_flags "${link_flags} --preload-file ${resource}@/media/${resource_name}")
        endforeach ()
        set_target_properties(${GAME_NAME} PROPERTIES LINK_FLAGS ${link_flags})
    endif()

    # Currently, Dawn can only be linked dynamically
    #set_target_properties(${GAME_NAME} PROPERTIES COMPILE_FLAGS "-DDW_DLL")

    # LuaJIT on 64-bit OS X requires these linker flags for executables
    if(APPLE)
        set_target_properties(${GAME_NAME} PROPERTIES LINK_FLAGS "-pagezero_size 10000 -image_base 100000000")
    endif()

    # Setup macOS app bundle.
    if(APPLE)
        set(BUNDLE_DIR ${OUT_DIR}/${GAME_NAME}.app)
        set(FRAMEWORKS_DIR ${BUNDLE_DIR}/Contents/Frameworks)
        set(RESOURCES_DIR ${BUNDLE_DIR}/Contents/Resources)
        add_custom_command(TARGET ${GAME_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${RESOURCES_DIR})
    endif()

    # Copy over shared library dependencies
    get_target_property(LIB_DIR DwEngine LIBRARY_OUTPUT_DIRECTORY)
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

enable_testing()
add_subdirectory(test)


###############################################
# Installation
###############################################
# TODO

