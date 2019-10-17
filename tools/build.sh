#!/usr/bin/env bash
mkdir build
pushd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build engine
cmake --build . --target Dawn

# Build examples
cmake --build . --target Sandbox
cmake --build . --target RendererTest

popd
