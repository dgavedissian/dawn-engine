#!/bin/bash
PATHS=(
    cmake
    src/dawn
    src/renderer-test
    src/sandbox
    test
    tools
)
cloc "${PATHS[@]}"
