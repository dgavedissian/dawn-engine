# Dawn Engine
# Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
cmake_minimum_required(VERSION 3.0)

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

# Workaround for 'warning C4577: 'noexcept' used with no exception handling mode specified;
# termination on exception is not guaranteed. Specify /EHsc'
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif(MSVC)

###############################################
# Helpful functions
###############################################

function(add_game GAME_NAME SOURCE_FILES)
    set(OUT_DIR ${CMAKE_SOURCE_DIR}/bin)

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
    get_target_property(DAWN_INCLUDE_DIRS DwCore INCLUDE_DIRECTORIES)
    target_include_directories(${GAME_NAME} PRIVATE
        ${DAWN_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR})

    # Add libraries
    target_link_libraries(${GAME_NAME} DwCore)
    set_output_dir(${GAME_NAME} ${OUT_DIR})
    set_target_properties(${GAME_NAME} PROPERTIES DEBUG_POSTFIX _d)

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
    get_target_property(LIB_DIR DwCore LIBRARY_OUTPUT_DIRECTORY)
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

