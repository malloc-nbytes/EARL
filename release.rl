module Release

import "std/system.rl"; as sys
import "std/script.rl"; as scr
import "std/colors.rl"; as clrs

set_flag("-xe");

let VERSION, TAG_MSG, COMMIT_MSG = ("", "", "");

@world fn final() {
    if TAG_MSG != "" {
        println(clrs::Tfc.Green, "tagging", clrs::Te.Reset);
        $f"git tag -a {VERSION} -m '{TAG_MSG}'";
    }

    println(clrs::Tfc.Green, "updated README.html", clrs::Te.Reset);
    ```
    rm README.html
    emacs --batch README.org -f org-html-export-to-html
    ```;

    if COMMIT_MSG != "" {
        println(clrs::Tfc.Green, "committing", clrs::Te.Reset);
        $"git add .";
        $f"git commit -m '{COMMIT_MSG}'";
        $"git push";
    }

    println(clrs::Tfc.Green, "building tarball", clrs::Te.Reset);
    $f"tar -czvf EARL-{VERSION}-linux.tar.gz -C build .";
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

    println("updating README.html");
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
commit();
final();

println(clrs::Tfc.Green, "done", clrs::Te.Reset);
