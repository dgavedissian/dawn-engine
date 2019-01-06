# Dawn Engine
# Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)

# Policies needed by gtest.
cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0057 NEW)

set(TEST_FILES
    core/io/FileSystemTest.cpp
    core/io/FileTest.cpp
    core/io/StringInputStreamTest.cpp
    Testing.h)

add_executable(DwEngineTests ${TEST_FILES})
mirror_physical_directories(${TEST_FILES})
enable_cpp17(DwEngineTests)
target_link_libraries(DwEngineTests DwEngine gtest gtest_main)
target_include_directories(DwEngineTests SYSTEM PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    # TODO: Bring these in from the dependencies on DwEngine
    ${CMAKE_SOURCE_DIR}/src/dawn
    ${INCLUDE_DIRS})
set_target_properties(DwEngineTests PROPERTIES DEBUG_POSTFIX "")

include(GoogleTest)
gtest_add_tests(
    TARGET DwEngineTests
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
