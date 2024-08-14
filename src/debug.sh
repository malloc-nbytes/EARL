#!/bin/bash

set -xe

g++ -ggdb -O0 -DDEBUG -Iinclude/ -o earl-debug-build *.cpp primitives/*.cpp grammar/*.cpp member-intrinsics/*.cpp include/shared-scope.hpp
