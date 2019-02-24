Dawn Engine
===========
[![Build Status](https://travis-ci.org/davedissian/dawnengine.svg?branch=develop)](https://travis-ci.org/davedissian/dawnengine)

A game engine written in C++14 designed for powering space games with galactic sized game worlds.

Build Process
-------------

Dawn Engine uses CMake for generating build scripts. The following compilers are supported:

* Microsoft Visual Studio 2015 and above.
* Xcode 8+ targeting macOS 10.11.0 or later.
* GCC 5+
* Clang 3.4+
* Emscripten

The source code is entirely self contained, using CMakes `FetchContent` feature to pin dependencies.
