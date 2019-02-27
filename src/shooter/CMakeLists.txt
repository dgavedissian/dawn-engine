# Dawn Engine
# Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)

set(SOURCE_FILES
    Main.cpp
    CProjectile.cpp
    CProjectile.h
    CShipEngines.cpp
    CShipEngines.h
    CWeapon.cpp
    CWeapon.h
    Ship.cpp
    Ship.h
    ShipCameraController.cpp
    ShipCameraController.h
    ShipFlightComputer.cpp
    ShipFlightComputer.h
    ShooterGameMode.cpp
    ShooterGameMode.h)

add_dawn_executable(DwShooter
    SRCS ${SOURCE_FILES}
    RESOURCE_PKGS ${CMAKE_SOURCE_DIR}/media/shooter)
