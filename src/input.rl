module Main

import "std/system.rl"; as sys

foreach c in "a" {
    println(c);
}

# $"ls" |> let files;
# foreach f in files {
#     println(f);
# }
