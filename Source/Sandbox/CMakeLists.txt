# Dawn Engine
# Copyright (c) 2012-2015 David Avedissian (avedissian.david@gmail.com)

set(H_FILES
    SandboxState.h)

set(CPP_FILES
    Main.cpp
    SandboxState.cpp)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../Dawn
    ${INCLUDE_DIRS})
add_game(Sandbox ${CPP_FILES} ${H_FILES})
target_link_libraries(Sandbox Dawn)
