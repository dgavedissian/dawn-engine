# Dawn Engine
# Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)

set(SOURCE_FILES
    Main.cpp
    CProjectile.cpp
    CProjectile.h
    CShipEngines.cpp
    CShipEngines.h
    Ship.cpp
    Ship.h
    ShipCameraController.cpp
    ShipCameraController.h
    ShipFlightComputer.cpp
    ShipFlightComputer.h
    ShooterGameMode.cpp
    ShooterGameMode.h)

add_game(Shooter "${SOURCE_FILES}")
