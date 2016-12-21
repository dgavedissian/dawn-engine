#!/bin/sh
find src/dawn src/sandbox -name "*.h" -exec clang-format -i {} \;
find src/dawn src/sandbox -name "*.cpp" -exec clang-format -i {} \;
find src/dawn src/sandbox -name "*.inl" -exec clang-format -i {} \;
