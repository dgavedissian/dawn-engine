#!/bin/bash
PATHS=(
    cmake
    src/dawn
    src/examples
    src/sandbox
    src/shooter
    src/viewer
    test
    tools
)
cloc "${PATHS[@]}"
