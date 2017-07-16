#!/bin/sh
if hash clang-format-3.8 2>/dev/null; then
    CLANG_FORMAT=clang-format-3.8
else
    CLANG_FORMAT=clang-format
fi

find src/dawn src/sandbox src/renderer-test -name "*.h" -exec $CLANG_FORMAT -i {} \;
find src/dawn src/sandbox src/renderer-test -name "*.cpp" -exec $CLANG_FORMAT -i {} \;
