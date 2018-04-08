# Dawn Engine
# Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)

set(SOURCE_FILES
    Main.cpp
    Ship.cpp
    Ship.h
        ShipEngines.cpp
        ShipEngines.h)

add_game(Shooter "${SOURCE_FILES}")
