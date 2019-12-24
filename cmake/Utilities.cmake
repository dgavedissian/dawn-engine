# Useful CMake utility functions
# Last Updated: 06/01/2019
# Copyright (c) 2015-19 David Avedissian (git@dga.dev)
cmake_minimum_required(VERSION 3.1)

# Set the output location explicitly
macro(set_output_dir TARGET OUTDIR)
    set_target_properties(${TARGET} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${OUTDIR}
        LIBRARY_OUTPUT_DIRECTORY ${OUTDIR}
        ARCHIVE_OUTPUT_DIRECTORY ${OUTDIR})
    foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
        set_target_properties(${TARGET} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${OUTDIR}
            LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${OUTDIR}
            ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${OUTDIR})
    endforeach()
endmacro()

# Macro to enable maximum warnings
macro(enable_maximum_warnings TARGET)
    if(MSVC)
        target_compile_options(${TARGET} PUBLIC /W4)
    elseif(UNIX)
        target_compile_options(${TARGET} PUBLIC -Wall -Wextra -pedantic -Wuninitialized -Wfloat-equal
                                                -Woverloaded-virtual -Wno-unused-parameter)
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            target_compile_options(${TARGET} PUBLIC -Wno-nested-anon-types -Wno-gnu-anonymous-struct)
        endif()
    endif()
endmacro()
