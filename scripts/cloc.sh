#!/bin/bash
PATHS=(
    cmake
    src
    test
    tools
)
cloc "${PATHS[@]}"
