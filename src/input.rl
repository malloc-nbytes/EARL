module Main

fn f(cl) {
    println(cl('a'));
}

f(== 'b');

# import "std/parsers/toml.rl"; as P

# let d = P::parse(env("HOME") + "/.earlmgr");

# foreach dep in d["dependencies.sstream"].unwrap()["deps"].unwrap() {
#     println(dep);
# }

# P::dump(d);
