# Useful CMake utility functions
# Last Updated: 4/9/2015
# Copyright (c) 2015 David Avedissian (avedissian.david@gmail.com)

# Create a vcproj userfile which correctly runs the binary in a specified working directory when debugging
function(create_vcproj_userfile TARGETNAME)
    cmake_policy(SET CMP0053 NEW)
    if(MSVC)
        set(WORKINGDIR ${ARGV1})
        set(VCPROJ_TEMPLATE "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
                            "<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">"
                            "<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">"
                            "<LocalDebuggerCommand>$(TargetPath)</LocalDebuggerCommand>"
                            "<LocalDebuggerWorkingDirectory>@CMAKE_CURRENT_SOURCE_DIR@/${WORKINGDIR}</LocalDebuggerWorkingDirectory>"
                            "<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor></PropertyGroup>"
                            "<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">"
                            "<LocalDebuggerCommand>$(TargetPath)</LocalDebuggerCommand>"
                            "<LocalDebuggerWorkingDirectory>@CMAKE_CURRENT_SOURCE_DIR@/${WORKINGDIR}</LocalDebuggerWorkingDirectory>"
                            "<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor></PropertyGroup>"
                            "<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='RelWithDebInfo|Win32'\">"
                            "<LocalDebuggerCommand>$(TargetPath)</LocalDebuggerCommand>"
                            "<LocalDebuggerWorkingDirectory>@CMAKE_CURRENT_SOURCE_DIR@/${WORKINGDIR}</LocalDebuggerWorkingDirectory>"
                            "<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor></PropertyGroup>"
                            "<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='MinSizeRel|Win32'\">"
                            "<LocalDebuggerCommand>$(TargetPath)</LocalDebuggerCommand>"
                            "<LocalDebuggerWorkingDirectory>@CMAKE_CURRENT_SOURCE_DIR@/${WORKINGDIR}</LocalDebuggerWorkingDirectory>"
                            "<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor></PropertyGroup>"
                            "</Project>")
        string(REPLACE ";" "\n" VCPROJ_TEMPLATE_STR "${VCPROJ_TEMPLATE}")
        string(CONFIGURE ${VCPROJ_TEMPLATE_STR} VCPROJ_TEMPLATE_STR_OUT @ONLY)
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${TARGETNAME}.vcxproj.user ${VCPROJ_TEMPLATE_STR_OUT})
    endif()
endfunction()

# Mirror the directory structure in virtual directory based projects
function(mirror_physical_directories)
    foreach(FILE ${ARGN})
        get_filename_component(PARENT_DIR "${FILE}" PATH)
        string(REPLACE "/" "\\" GROUP "${PARENT_DIR}")
        source_group("${GROUP}" FILES "${FILE}")
    endforeach()
endfunction()

# Set the output location explicitly
macro(set_output_dir OUTDIR)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTDIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTDIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTDIR})
    foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${OUTDIR})
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${OUTDIR})
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${OUTDIR})
    endforeach()
endmacro()

# Macro to enable C++11 and on OS X enable libc++
macro(enable_cpp11)
    if(UNIX)
        add_compile_options(-std=c++11)

        # If on OS X, use the libc++ standard library which is much more C++11 complete
        if(APPLE)
            add_compile_options(-stdlib=libc++)
            if(XCODE)
                set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
                set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
            endif()
        endif()
    endif()
endmacro()

# Macro to enable maximum warnings
macro(enable_maximum_warnings)
    if(MSVC)
        add_definitions(-D_SCL_SECURE_NO_WARNINGS)
        add_compile_options(/W4)
    elseif(UNIX)
        add_compile_options(-Wall -Wextra -pedantic -Wuninitialized -Wfloat-equal -Woverloaded-virtual
                            -Wno-deprecated-declarations -Wno-unused-parameter)
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            add_compile_options(-Wno-nested-anon-types -Wno-gnu-anonymous-struct)
        endif()
    endif()
endmacro()
