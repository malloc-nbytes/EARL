module Release

import "std/system.rl"; as sys
import "std/script.rl"; as scr

set_flag("-xe");

let VERSION = "";

fn build_tarball(version) {
    $f"tar -czvf EARL-{version}-linux.tar.gz -C build .";
}

fn commit() {
    println("committing");
    let commit_msg = input("Final commit message: ");
    if commit_msg == "" {
        panic("empty commit message");
    }
    $"git add .";
    $f"git commit -m '{commit_msg}'";
    $"git push";
}

fn update_readme() {
    println("checking README.{org,html}");

    $"git diff main -- README.org" |> let modified_org;

    if len(modified_org) == 0 {
        panic("README.org is has not been updated");
    }

    println("updating README.html");
    if !scr::program_exists("emacs") {
        panic("emacs is not available on the system, failing update");
    }
    $"rm README.html";
    $"emacs --batch README.org -f org-html-export-to-html";
}

fn tag(version) {
    if !case input("create a tag? [y/n]: ") of {
            "Y" = true;  "y" = true;
            "N" = false; "n" = false;
             _  = false; } {
        return;
    }

    let tag_msg = input("tag message: ");
    if tag_msg == "" {
        panic("empty tag message");
    }
    $f"git tag -a {version} -m '{tag_msg}'";
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
tag(VERSION);
update_readme();
commit();
build_tarball(VERSION);

println("done");
