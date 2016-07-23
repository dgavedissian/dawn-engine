dawnengine [![Build Status](https://travis-ci.org/davedissian/dawnengine.svg?branch=develop)](https://travis-ci.org/davedissian/dawnengine)
===========

A game engine written in C++ designed for powering space games which require galactic sized game worlds to millimetre precision. I've worked on this as my own project for a number of years to learn about game engine design, and is in the process of being heavily refactored to strip out some large external libraries as well as improve the code design.

The library requires SDL2 as a dependency, and on Linux it also requires libxaw.

Build Process
-------------
1. Run CMake, set the source directory to <dir>/dawnengine, and the binary directory to <dir>/dawnengine/build. (replacing <dir> with where this file is located).
    * For Linux and OS X, just execute `cd dawnengine && mkdir build && cd build && cmake ..`
2. For Windows, enter the build directory and open the produced DawnEngine.sln file. For Linux and OS X, just run `make` in the build directory. In OS X, for the time being, the build will fail at some point. Just run `make` a second time and it will work fine.
