# Dawn Engine
# Written by David Avedissian (c) 2012-2019 (git@dga.dev)

add_dawn_executable(
    DwSandbox
    SRCS Main.cpp ../dawn/scene/space/PlanetLod.cpp ../dawn/scene/space/PlanetLod.h
    RESOURCE_PKGS ${CMAKE_SOURCE_DIR}/media/sandbox)
