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

### This file is for setting up a release.
### It:
###   1. builds the project
###   2. tags the project
###   3. updates the README.html and will check for README.org enforcement change
###   4. optionally update EARL-web (does not commit, push, or deploy)
###   5. commits and pushes the final results.

module Release

import "std/system.rl"; as sys
import "std/script.rl"; as scr
import "std/colors.rl"; as clrs

set_flag("-xe");

let VERSION, TAG_MSG, COMMIT_MSG = ("", "", "");

fn log(msg) {
    println(
        clrs::Tfc.Green,
        clrs::Te.Bold,
        "---------- ",
        msg,
        " ----------",
        clrs::Te.Reset
    );
}

@world fn final() {
    if TAG_MSG != "" {
        log("tagging");
        $f"git tag -a {VERSION} -m '{TAG_MSG}'";
    }

    log("updating README.html");

    ```
    rm README.html
    emacs --batch README.org -f org-html-export-to-html
    ```;

    if COMMIT_MSG != "" {
        log("committing");
        $"git add .";
        $f"git commit -m '{COMMIT_MSG}'";
    }

    if case input("push to remote? [Y/n]: ") of {
            "Y" = true;  "y" = true;
            "N" = false; "n" = false;
             _  = true; } {
        $"git push";
    }

    log("building tarball");
    $f"tar -czvf EARL-{VERSION}-linux.tar.gz -C build .";
}

fn update_EARL_web() {
    if !case input("update EARL-web StdLib modules? [Y/n]: ") of {
            "Y" = true;  "y" = true;
            "N" = false; "n" = false;
             _  = true; } {
        return;
    }

    ```
    cd src
    earl stdlib-docs-gen.rl -- jsx
    ```;
    $"mv ./src/StdLibModules.jsx " + env("HOME") + "/dev/EARL-web/src/code-snippets/StdLibModules.jsx";
}

@world fn commit() {
    if !case input("create a final commit? [Y/n]: ") of {
            "Y" = true;  "y" = true;
            "N" = false; "n" = false;
             _  = true; } {
        return;
    }

    COMMIT_MSG = input("Final commit message: ");
    if COMMIT_MSG == "" {
        panic("empty commit message");
    }
}

fn update_readme() {
    if !case input("Enforce README.org/html update? [Y/n]: ") of {
            "Y" = true;  "y" = true;
            "N" = false; "n" = false;
             _  = true; } {
        return;
    }

    $"git diff main -- README.org" |> let modified_org;

    if len(modified_org) == 0 {
        panic("README.org is has not been updated");
    }

    if !scr::program_exists("emacs") {
        panic("emacs is not available on the system, failing update");
    }
}

@world fn tag() {
    if !case input("create a tag? [Y/n]: ") of {
            "Y" = true;  "y" = true;
            "N" = false; "n" = false;
             _  = true; } {
        return;
    }

    println("Note: current EARL version in ./build/: ", VERSION);
    TAG_MSG = input("tag message: ");
    if TAG_MSG == "" {
        panic("empty tag message");
    }
}

@world fn build() {
    ```
    cd build
    make clean
    make -j$(nproc)
    make test
    cd ..
    ```;
    $"./build/earl -v" |> let version_info;
    VERSION = 'v' + version_info.split(" ")[1];
}

build();
tag();
update_readme();
update_EARL_web();
commit();
final();

log("done");
