Dawn Engine
===========
[![Build Status](https://travis-ci.org/dgavedissian/dawn-engine.svg?branch=develop)](https://travis-ci.org/dgavedissian/dawnengine)

A game engine written in C++17 designed for powering space games with galactic sized game worlds.

Build Process
-------------

Dawn Engine uses CMake 3.14+ for generating build scripts. The following compilers are supported:

* Microsoft Visual Studio 2017 and above.
* Xcode 10+ targeting macOS 10.14.0 or later.
* GCC 7+
* Clang 6.0+
* Emscripten

The source code is entirely self contained, using CMakes `FetchContent` feature to pin dependencies.

Note that on Linux, OpenGL development libraries and X11 development libraries are required. They can be installed by
running:
```
sudo apt-get install xorg-dev libglu1-mesa-dev
```