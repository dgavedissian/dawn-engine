#!/bin/bash
if [ ! -f build/src/3rdparty/shaderc ]; then
    echo "Error: shaderc not found. Please build dawnengine first."
    exit 1
fi
./build/src/3rdparty/shaderc
