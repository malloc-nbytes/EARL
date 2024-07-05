# Use This file to run a debug build of EARL.
# It builds with no optimizations, the DEBUG flag
# defined, and debug symbols enabled.
# Once it is compiled, it will launch EARL with
# the file ../input.earl as the input file.

#!/bin/bash

set -xe

if [ "$#" -eq 0 ]; then
    echo "Usage: ./debug <files...>"
    exit 1
fi

g++ -Iinclude/ -Wreturn-type -O0 -DDEBUG -std=c++17 -g -o earl-debug-build *.cpp primitives/*.cpp member-intrinsics/*.cpp grammar/*.cpp include/scope.hpp
gdb --args ./earl-debug-build "$@"
