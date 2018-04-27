# Dawn Engine
# Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)

set(SOURCE_FILES
    Main.cpp
    Projectile.cpp
    Projectile.h
    Ship.cpp
    Ship.h
    ShipEngines.cpp
    ShipEngines.h
    ShooterGameMode.cpp
    ShooterGameMode.h)

add_game(Shooter "${SOURCE_FILES}")
