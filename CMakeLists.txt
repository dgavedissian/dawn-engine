# Dawn Engine
# Written by David Avedissian (c) 2012-2019 (git@dga.dev)
cmake_minimum_required(VERSION 3.14)

project(Dawn)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_MACOSX_RPATH 1)

if(APPLE)
    if(CMAKE_OSX_DEPLOYMENT_TARGET STREQUAL "")
        set(CMAKE_OSX_DEPLOYMENT_TARGET 10.12)
    endif()
endif()

option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." ON)
if(${FORCE_COLORED_OUTPUT})
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options(-fdiagnostics-color=always)
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options(-fcolor-diagnostics)
    endif()
endif()

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

# ccache
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CUDA_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

if(MSVC)
    add_compile_options(
        # Workaround for 'warning C4577: 'noexcept' used with no exception handling mode specified;
        # termination on exception is not guaranteed. Specify /EHsc'
        /EHsc
    )
    add_compile_definitions(
        # Required to suppress warnings if using unsafe C functions (like printf) with /W3 or above.
        _SCL_SECURE_NO_WARNINGS
        # As of MSVC 2017 15.8, we need to set this define to use "correct" alignment behaviour.
        _ENABLE_EXTENDED_ALIGNED_STORAGE
    )
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

    # Compile in C++17 mode
    target_compile_features(${GAME_NAME} PUBLIC cxx_std_17)
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

    #set_target_properties(${GAME_NAME} PROPERTIES COMPILE_FLAGS "-DDW_DLL")

    # Currently, macOS STL requires this to be defined to use std::visit before macOS 10.14.
    # See: https://stackoverflow.com/questions/52310835/xcode-10-call-to-unavailable-function-stdvisit
    if(APPLE)
        set_target_properties(${GAME_NAME} PROPERTIES COMPILE_FLAGS "-D_LIBCPP_DISABLE_AVAILABILITY")
    endif()

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
# Installation
###############################################
# TODO

