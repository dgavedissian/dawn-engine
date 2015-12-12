Dawn Engine
===========

A game engine written in C++ designed for powering space games which require galactic sized game worlds to millimetre precision. I've worked on this as my own project for a number of years to learn about game engine design, so some decision decisions are a bit dumb and are in the process of eventually being refactored.

Build Process
-------------
1. Build dependencies as specified in the instructions below.
2. Run CMake, set the source directory to .../dawnengine, and the binary directory to .../dawnengine/build. (replacing ... with where this repo was cloned).
    * For Linux and OS X, just execute `cd dawnengine && mkdir build && cd build && cmake ..`
3. For Windows, enter the build directory and open the produced DawnEngine.sln file. For Linux and OS X, just run make in the build directory.

Dependencies
-------------

The dependencies source package can be downloaded from:
https://dl.dropboxusercontent.com/u/1490400/github/dawnengine-3rdparty-src.7z

Extract this folder on your system, and run the corresponding build script depending on your platform.

Linux
------------
For all Linux distros, run ./build-unix.sh. It currently requires Clang to be installed, but if you only have GCC then feel free to delete the "export CC" and "export CXX" lines from the script. You may need to install a few other random development libraries, refer to your distros documentation on what packages you need to install depending on what errors you get. Once it builds successfully you should have a "linux-package" directory which then needs to be copied to dawnengine/dependencies.

Mac OS X
------------
For OS X, just simply run ./build-unix.sh and it should take care of everything. Once it builds successfully you should have a "macosx-package" directory which then needs to be copied to dawnengine/dependencies.

Windows
------------
For Windows, multiple batch scripts are provided depending on your version of Visual Studio. This will also result in a "vs20**-package" directory which then needs to be copied to dawnengine/dependencies.
