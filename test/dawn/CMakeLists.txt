# Dawn Engine
# Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)

set(TEST_FILES
    io/FileTest.cpp
    Testing.h)

add_executable(DawnTest ${TEST_FILES})
enable_cpp14(DawnTest)
target_link_libraries(DawnTest Dawn gtest gtest_main)
target_include_directories(DawnTest SYSTEM PRIVATE . ${CMAKE_SOURCE_DIR}/src/dawn ${INCLUDE_DIRS})
