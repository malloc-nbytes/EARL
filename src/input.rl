module Main

import "std/io.rl"; as io
import "std/system.rl"; as sys

println(sys::name_and_ext("test.txt"));
println(sys::name_and_ext("test"));
println(sys::name_and_ext(".txt"));
println(sys::name_and_ext("."));
