module Debug

import "std/system.rl";
import "std/colors.rl";

set_flag("-xe");

@world fn walk(path) {
    println(path);
    foreach f in System::ls(path) {
        if f == "./include" || f == "./std" {
            continue;
        }

        if System::isdir(f) {
            walk(f);
        } else {
            if f.split(".").back() == "c" {
                println("    ", f);
                c_files.append(f);
            }
        }
    }
}

# Init
let build_name = "earl-debug-build";
let c_files = [];
let flags = ["-Iinclude/", "-Wextra", "-Wall", "-O0", "-ggdb", f"-o {build_name}"];
let compiler = "clang";

# Get all src files
walk(".");

# Fold all lists
let SRC, FLAGS, COMPILER = (
    c_files.fold(|k, acc| {acc + ' ' + k;}, ""),
    flags.fold(|k, acc| {acc + ' ' + k;}, ""),
    compiler,
);

# The build command
let cc = COMPILER + ' ' + FLAGS + ' ' + SRC;

# Run the build command
$cc;

println(Colors::Tfc.Green, "\n--- DONE ---\n", Colors::Te.Reset);
$f"gdb ./{build_name}";
