module Main

# This script takes a program name from the cli
# and will grep for all running processes that
# match what the user entered. You can then
# pick and choose which procs to kill, or
# you can just kill them all.

import "std/script.rl"; as scr

fn handle_input(num, candidates) {
    if num == 'a' || num == "all" {
        let pids = candidates.map(|info| {return info[0];});
        for i in 0 to len(pids) {
            println("killing: ", candidates[i]);
            scr::kill_pid(pids[i]);
        }
    }
    else if num == 'q' || num == "quit" {
        println("quitting...");
        exit(0);
    }
    else {
        let idx = int(num);
        if idx < 0 || idx >= len(candidates) {
            panic(f"index {idx} is out of range of length ", len(candidates));
        }
        println("killing: ", candidates[idx]);
        scr::kill_pid(candidates[idx][0]);
    }
}

if len(argv()) < 2 {
    panic("Usage: ", argv()[0], " <proc>");
}

let max_display_chars, program_name = (
    50,        # Truncate output because it can get long
    argv()[1], # User supplied CLI arg
);

# Get all PIDs and process names
let candidates = scr::get_pid(program_name)
    .rev()[1:] # Cut off this earl proc
    .rev();    # Undo rev

if len(candidates) == 0 {
    println("no processes found");
    exit(0);
}

println(
    "Choose a number or list of numbers, `a` for all, or `q` to quit"
);

for i in 0 to len(candidates) {
    let pid, cmd = (
        candidates[i][0],
        candidates[i][1],
    );
    if len(cmd) > max_display_chars {
        println(f"  {i}: ({pid}, ", cmd.substr(0, max_display_chars), "...)");
    }
    else {
        println(f"  {i}: ({pid}, {cmd})");
    }
}

# Handle user input
input("[a(ll), q(uit), ", 0, '-', len(candidates)-1, "]: ")
    .split(" ")                   # could have multiple inputs
    .filter(|s|{return s != "";}) # remove empty entries
    .foreach(|k|{handle_input(k, candidates);});
