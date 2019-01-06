Dawn Engine
===========
[![Build Status](https://travis-ci.org/davedissian/dawnengine.svg?branch=develop)](https://travis-ci.org/davedissian/dawnengine)

A game engine written in C++17 designed for powering space games with galactic sized game worlds.

Build Process
-------------

Dawn Engine uses CMake for generating build scripts. The following compilers are supported:

* Microsoft Visual Studio 2017 and above.
* Xcode 9.3+ targeting macOS 10.13.0 or later.
* GCC 7+
* Clang 5+
* Emscripten

The source code is entirely self contained, using CMakes `FetchContent` feature to pin dependencies.
