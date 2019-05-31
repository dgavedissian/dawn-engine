# Dawn Engine
# Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
cmake_minimum_required(VERSION 3.11)

set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CFlagOverrides.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CXXFlagOverrides.cmake)

project(Dawn)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_MACOSX_RPATH 1)

if(APPLE)
    if(CMAKE_OSX_DEPLOYMENT_TARGET STREQUAL "")
        set(CMAKE_OSX_DEPLOYMENT_TARGET 10.12)
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

function(create_pkg_manifest MANIFEST_NAME)
    cmake_parse_arguments(
        MANIFEST
        ""
        ""
        "DIRS;PKGS"
        ${ARGN}
    )

    set(PKG_ARGS --output "${MANIFEST_NAME}")

    list(LENGTH MANIFEST_DIRS MANIFEST_DIRS_SIZE)
    if(${MANIFEST_DIRS_SIZE} GREATER 0)
        math(EXPR MANIFEST_DIRS_SIZE "${MANIFEST_DIRS_SIZE}-1")
    endif()
    foreach(INDEX RANGE 0 ${MANIFEST_DIRS_SIZE} 2)
        list(GET MANIFEST_DIRS ${INDEX} PACKAGE_NAME)
        math(EXPR PATH_INDEX "${INDEX}+1")
        list(GET MANIFEST_DIRS ${PATH_INDEX} PACKAGE_PATH)
        set(PKG_ARGS ${PKG_ARGS} --dir "${PACKAGE_NAME}|${PACKAGE_PATH}")
    endforeach()

    add_custom_command(
        OUTPUT ${MANIFEST_NAME} ${MANIFEST_NAME}__always_rerun
        DEPENDS pkg
        COMMAND $<TARGET_FILE:pkg>
        ARGS ${PKG_ARGS}
        COMMENT "Generate manifest file"
        VERBATIM
    )
endfunction()

function(add_dawn_executable GAME_NAME)
    cmake_parse_arguments(
        TARGET # prefix of output variables
        "" # list of names of the boolean arguments (only defined ones will be true)
        "" # list of names of mono-valued arguments
        "SRCS;RESOURCE_DIRS" # list of names of multi-valued arguments (output variables are lists)
        ${ARGN} # arguments of the function to parse, here we take the all original ones
    )

    # Get the source dir of the engine (to get the media files directory).
    get_target_property(DAWN_CMAKE_SOURCE_DIR DwEngine SOURCE_DIR)

    # Append base resource package.
    set(TARGET_RESOURCE_DIRS base ${DAWN_CMAKE_SOURCE_DIR}/media ${TARGET_RESOURCE_DIRS})

    message(STATUS "add_dawn_executable: Adding ${GAME_NAME}.")
    message(STATUS "add_dawn_executable:   Sources: ${TARGET_SRCS}")
    message(STATUS "add_dawn_executable:   Resources: ${TARGET_RESOURCE_DIRS}")

    set(OUT_DIR ${CMAKE_SOURCE_DIR}/bin)

    # Executable type
    if(WIN32)
        set(TYPE WIN32)
    elseif(APPLE)
        set(TYPE MACOSX_BUNDLE)
    endif()

    # Add executable
    add_executable(${GAME_NAME} ${TYPE} ${TARGET_SRCS})
    add_dependencies(${GAME_NAME} ${GAME_NAME}Manifest)

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
    if(EMSCRIPTEN)
        set_target_properties(${GAME_NAME} PROPERTIES SUFFIX .html)
        set(link_flags "-s DEMANGLE_SUPPORT=1 -s TOTAL_MEMORY=256MB -s ALLOW_MEMORY_GROWTH=1 --no-heap-copy")
        if(DW_DISABLE_WASM)
            set(link_flags "${link_flags} -s WASM=0")
        endif()
    endif()

    # Create manifest
    if(EMSCRIPTEN)
        # Include resources.
        list(LENGTH RESOURCE_DIRS resource_dirs_size)
        if(${resource_dirs_size} GREATER 0)
            math(EXPR resource_dirs_size "${resource_dirs_size}-1")
        endif()
        foreach(index RANGE 0 ${resource_dirs_size} 2)
            list(GET RESOURCE_DIRS ${index} package_name)
            math(EXPR path_index "${index}+1")
            list(GET RESOURCE_DIRS ${path_index} package_path)
            set(link_flags "${link_flags} --preload-file ${package_path}@/media/${package_name}")
            set(emscripten_resource_dirs ${emscripten_resource_dirs} ${package_name} /media/${package_name})
        endforeach()

        # Create manifest that points to "/media/<resource>" directories.
        create_pkg_manifest(${GAME_NAME}.manifest DIRS ${emscripten_resource_dirs})
        set(link_flags "${link_flags} --preload-file ${GAME_NAME}.manifest")
        add_custom_target(${GAME_NAME}Manifest DEPENDS ${GAME_NAME}.manifest)
    else()
        create_pkg_manifest(${GAME_NAME}.debug.manifest DIRS ${TARGET_RESOURCE_DIRS})
        add_custom_target(${GAME_NAME}Manifest DEPENDS ${GAME_NAME}.debug.manifest)
    endif()

    if(EMSCRIPTEN)
        set_target_properties(${GAME_NAME} PROPERTIES LINK_FLAGS ${link_flags})
    endif()

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

