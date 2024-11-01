#!/usr/local/bin/earl

module Debug

let install = len(argv()) > 1 && argv()[1] == "install";
let CXX_DEBUG_BUILD_NAME = "earl-debug-build";
let CXX = "g++";
let CXX_FLAGS = f"-ggdb -O0 -DDEBUG -Iinclude/ -o {CXX_DEBUG_BUILD_NAME}";
let CXX_SRC = "*.cpp primitives/*.cpp grammar/*.cpp member-intrinsics/*.cpp include/shared-scope.hpp";
let EARL_FILES = "*.rl std/*.rl";

println(f"install: {install}");
println(f"CXX_DEBUG_BUILD_NAME: {CXX_DEBUG_BUILD_NAME}");
println(f"CXX: {CXX}");
println(f"CXX_FLAGS: {CXX_FLAGS}");
println(f"CXX_SRC: {CXX_SRC}");
println("LOC");
$f"cat {CXX_SRC} ./include/*.hpp {EARL_FILES} | wc -l";
println("FILES");
$f"ls {CXX_SRC} ./include/*.hpp {EARL_FILES} | wc -l";

$f"echo compiling {CXX_DEBUG_BUILD_NAME}... && {CXX} {CXX_FLAGS} {CXX_SRC}";

if install {
    $f"sudo cp {CXX_DEBUG_BUILD_NAME} /usr/local/bin";
}
