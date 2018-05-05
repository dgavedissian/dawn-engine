# Dawn Engine
# Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)

set(SOURCE_FILES
    Main.cpp
    Projectile.cpp
    Projectile.h
    Ship.cpp
    Ship.h
    ShipCameraController.cpp
    ShipCameraController.h
    ShipEngines.cpp
    ShipEngines.h
    ShipFlightComputer.cpp
    ShipFlightComputer.h
    ShooterGameMode.cpp
    ShooterGameMode.h)

add_dawn_executable(Shooter
    SRCS ${SOURCE_FILES}
    RESOURCE_PKGS ${CMAKE_SOURCE_DIR}/media/shooter)
