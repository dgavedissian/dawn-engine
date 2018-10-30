#!/bin/sh
if hash clang-format-6.0 2>/dev/null; then
    CLANG_FORMAT=clang-format-6.0
else
    CLANG_FORMAT=clang-format
fi

# Check the version is correct.
CLANG_FORMAT_VERSION=$(clang-format --version | cut -d " " -f 3)
if [[ ! $CLANG_FORMAT_VERSION =~ ^6.0 ]]; then
    echo "Unsupported clang-format version: $CLANG_FORMAT_VERSION"
    exit
fi

# Run clang-format against a set of paths.
PATHS=(
    src/dawn
    src/examples
    src/sandbox
    src/shooter
    src/viewer
)
find "${PATHS[@]}" -name "*.h" -exec $CLANG_FORMAT -i {} \;
find "${PATHS[@]}" -name "*.cpp" -exec $CLANG_FORMAT -i {} \;
