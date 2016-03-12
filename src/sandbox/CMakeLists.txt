# Dawn Engine
# Written by David Avedissian (c) 2012-2016 (git@davedissian.com)

set(H_FILES
    SandboxState.h)

set(CPP_FILES
    Main.cpp
    SandboxState.cpp)

add_game(Sandbox ${CPP_FILES} ${H_FILES})
