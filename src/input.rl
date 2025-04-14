module Main

import "std/io.rl"; as io
import "std/system.rl"; as sys

println(sys::get_all_files_by_ext("./include/", "hpp"));
