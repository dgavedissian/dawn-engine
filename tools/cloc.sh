#!/bin/bash
PATHS=(
    cmake
    src/dawn
    src/renderer-test
    src/sandbox
    src/shooter
    test
    tools
)
cloc "${PATHS[@]}"
