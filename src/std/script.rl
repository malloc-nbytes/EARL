# MIT License

# Copyright (c) 2023 malloc-nbytes

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

module Script

import "std/str.rl";
import "std/system.rl";

### Function
#-- Name: program_exists
#-- Parameter: progname: str
#-- Returns: str
#-- Description:
#--   Returns `true` if `progname` exists as a program on the
#--   machine or `false` if otherwise.
@pub fn program_exists(progname: str): bool {
    return System::cmd(f"which {progname} > /dev/null 2>&1") == 0;
}
### End

### Function
#-- Name: get_pid
#-- Parameter: proc: str
#-- Returns: list<tuple<int, str>>
#-- Description:
#--   Returns a list of pids and the process associated
#--   with it in the form of `[(pid1, proc1), (pid2, proc2), ..., (pidN, procN)]`.
@pub fn get_pid(proc: str): list {
    let output = System::cmdstr(f"ps aux | grep -i {proc}").split("\n");
    let res = [];

    let last_idx = 10;

    foreach o in output {
        let parts = o.split(" ").filter(|k|{return k != "";});
        if len(parts) < 10 {
            continue;
        }

        let pid = parts[1];
        let cmd_parts = parts[last_idx:];
        let cmd = "";

        for i in 0 to len(cmd_parts) {
            cmd += cmd_parts[i];
            if i != len(cmd_parts)-1 {
                cmd += ' ';
            }
        }

        res.append((int(pid), cmd));
    }

    # Remove grep entries
    if len(res) > 2 {
        res.pop(len(res)-1);
        res.pop(len(res)-1);
    }

    return res;
}
### End

### Function
#-- Name: kill_pid
#-- Parameter: pid: int
#-- Returns: unit
#-- Description:
#--   Kill the PID `pid`.
@pub fn kill_pid(pid: int): unit {
    System::cmd_wcheck(f"kill {pid}");
}
### End

### Function
#-- Name: kill_pids
#-- Parameter: pid: list<int>
#-- Returns: unit
#-- Description:
#--   Kill all PIDs `pids`.
@pub fn kill_pids(pids: list): unit {
    foreach pid in pids {
        kill_pid(pid);
    }
}
### End

### Function
#-- Name: wget
#-- Parameter: link: str
#-- Parameter: name: str
#-- Returns: unit
#-- Description:
#--   Performs a `wget` on `link` and saves it to `name`.
@pub fn wget(link: str, name: str): unit {
    if !program_exists("wget") {
        panic(__FUNC__, ": program `wget` does not exist on the system");
    }
    System::cmd_wcheck(f"wget -O {name} {link}");
}
### End

### Function
#-- Name: wget_wredirects
#-- Parameter: link: str
#-- Parameter: name: str
#-- Returns: unit
#-- Description:
#--   Performs a `wget` on `link` and saves it to `name`.
#--   Allows redirects.
@pub fn wget_wredirects(link: str, name: str): unit {
    if !program_exists("wget") {
        panic(__FUNC__, ": program `wget` does not exist on the system");
    }
    System::cmd_wcheck(f"wget --content-disposition -O {name} {link}");
}
### End

### Function
#-- Name: git_clone
#-- Parameter: link: str
#-- Parameter: depth: int
#-- Returns: unit
#-- Description:
#--   Performs a `git clone <link> --depth=<depth>`. To not
#--   specify a depth, put some number less than 1 as the `depth`.
@pub fn git_clone(link: str, depth: int): unit {
    if !program_exists("git") {
        panic(__FUNC__, ": program `git` does not exist on the system");
    }

    if depth < 1 {
        System::cmd_wcheck(f"git clone {link}");
    }
    else {
        System::cmd_wcheck(f"git clone {link} --depth={depth}");
    }
}
### End

### Function
#-- Name: whoami
#-- Returns: str
#-- Description:
#--   Return the user as a `str`.
@pub fn whoami(): str {
    let s = System::cmdstr("whoami");
    s.pop(len(s)-1); # newline at the end
    return s;
}
### End

### Function
#-- Name: all_running_procs
#-- Returns: list<dict<str>>
#-- Description:
#--   Return information on all running processes as a `dict<str>`
#--   in the form of:
#--     `dict["user"] = str`
#--     `dict["pid"] = str`
#--     `dict["cpu"] = str`
#--     `dict["mem"] = str`
#--     `dict["vsz"] = str`
#--     `dict["rss"] = str`
#--     `dict["tty"] = str`
#--     `dict["stat"] = str`
#--     `dict["start"] = str`
#--     `dict["time"] = str`
#--     `dict["cmd"] = str`
@pub fn all_running_procs(): list {
    if !program_exists("ps") {
        panic(__FUNC__, ": program `ps` does not exist on the system");
    }

    # [:1] to remove the legend
    let output, res = (
        System::cmdstr("ps aux").split("\n")[1:],
        [],
    );

    foreach entry in output {
        let parts = entry
            .split(" ")
            .filter(|k|{
                return k != "";
            });

        if len(parts) < 10 {
            continue;
        }

        let user = parts[0];
        let pid = parts[1];
        let cpu = parts[2];
        let mem = parts[3];
        let vsz = parts[4];
        let rss = parts[5];
        let tty = parts[6];
        let stat = parts[7];
        let start = parts[8];
        let time = parts[9];

        let cmd_parts = parts[10:];
        let cmd = "";
        for i in 0 to len(cmd_parts) {
            cmd.append(cmd_parts[i]);
            if i != len(cmd_parts)-1 {
                cmd += ' ';
            }
        }

        let d = {
            "user": user,
            "pid": pid,
            "cpu": cpu,
            "mem": mem,
            "vsz": vsz,
            "rss": rss,
            "tty": tty,
            "stat": stat,
            "start": start,
            "time": time,
            "cmd": cmd
        };

        res.append(d);
    }

    return res;
}
### End

### Function
#-- Name: launch_proc
#-- Parameter: proc: str
#-- Returns: unit
#-- Description:
#--   Launch the processes `proc`.
@pub fn launch_proc(proc: str): unit {
    System::cmd_wcheck(proc);
}
### End

### Function
#-- Name: launch_bgproc
#-- Parameter: proc: str
#-- Returns: unit
#-- Description:
#--   Launch the processes `proc` in the background.
@pub fn launch_bgproc(proc: str): unit {
    System::cmd_wcheck(proc+'&');
}
### End
