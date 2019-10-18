#!/usr/bin/env bash
PATHS=(
    cmake
    src
    test
    tools
)
cloc "${PATHS[@]}"
