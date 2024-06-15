#!/bin/bash

set -xe

g++ -Iinclude/ -std=c++17 -o main *.cpp include/scope.hpp
gdb --args ./main ../input.earl
