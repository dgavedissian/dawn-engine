# Dawn Engine
# Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)

add_dawn_executable(
    DwLuaShooter
    SRCS
        Main.cpp
    RESOURCE_PKGS
        ${CMAKE_CURRENT_SOURCE_DIR}/data
        ${CMAKE_SOURCE_DIR}/media/examples)
