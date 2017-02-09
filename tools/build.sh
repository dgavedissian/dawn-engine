#!/bin/bash
mkdir build
pushd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build engine
cmake --build . --target Dawn
cmake --build . --target shaderc

# Build examples
cmake --build . --target Sandbox

popd
