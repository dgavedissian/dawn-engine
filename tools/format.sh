#!/bin/sh
if hash clang-format-3.8 2>/dev/null; then
    CLANG_FORMAT=clang-format-3.8
else
    CLANG_FORMAT=clang-format
fi

PATHS=(
    src/dawn
    src/mesh-viewer
    src/renderer-test
    src/sandbox
    src/shooter
)

find "${PATHS[@]}" -name "*.h" -exec $CLANG_FORMAT -i {} \;
find "${PATHS[@]}" -name "*.cpp" -exec $CLANG_FORMAT -i {} \;
