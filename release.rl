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

    let TAG_MSG = input("tag message: ");
    if tag_msg == "" {
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
