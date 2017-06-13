# Dawn Engine
# Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)

set(SOURCE_FILES
    Main.cpp)

set(SHADER_SOURCE_FILES
    media/sandbox/shader)

add_game(Sandbox "${SOURCE_FILES}" "${SHADER_SOURCE_FILES}")
