# Dawn Engine
# Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)

# Policies needed by gtest.
cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0057 NEW)

set(TEST_FILES
    core/io/FileSystemTest.cpp
    core/io/FileTest.cpp
    core/io/StringInputStreamTest.cpp
    Testing.h)

add_executable(DawnTestRunner ${TEST_FILES})
mirror_physical_directories(${TEST_FILES})
enable_cpp14(DawnTestRunner)
target_link_libraries(DawnTestRunner Dawn gtest gtest_main)
target_include_directories(DawnTestRunner SYSTEM PRIVATE . ${CMAKE_SOURCE_DIR}/src/dawn ${INCLUDE_DIRS})

include(GoogleTest)
gtest_add_tests(
    TARGET DawnTestRunner
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
