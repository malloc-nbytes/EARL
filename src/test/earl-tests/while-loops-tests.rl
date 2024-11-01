module WhileLoopTests

import "std/assert.rl";
import "test-utils.rl";

Assert::FILE = __FILE__;

fn test_while_loop_return(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    fn aux(@ref i) {
        while i < 100 {
            if i == 50 { return; }
            i += 1;
        }
    }

    let i = 0;
    aux(i);

    Assert::eq(i, 50);
}

fn test_while_loop_count(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let s = 70000;
    let i, c = (0,0);
    while i < s {
        if i%2 == 0 { c+=1; }
        i += 1;
    }

    Assert::eq(i, s);
    Assert::eq(c, s/2);
}

fn test_while_loop_continue(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let i = 0;
    let c = 0;
    let s = 100;
    while i < s {
        if i%2 == 0 {
            i += 1;
            continue;
        }
        i += 1;
        c += 1;
    }

    Assert::eq(i, s);
    Assert::eq(c, s/2);
}

fn test_while_loop_break(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let i = 0;
    while i < 10 {
        if i == 5 { break; }
        i += 1;
    }
    Assert::eq(i, 5);
}

fn test_while_loop_big_down(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let i = 100000;
    while i > 0 {
        i -= 1;
    }
    Assert::eq(i, 0);
}

fn test_while_loop_big_up(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let cap = 100000;
    let i = 0;
    while i < cap {
        i += 1;
    }
    Assert::eq(i, cap);
}

fn test_basic_while_loop_up_wstride(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let i = 0;
    while i < 10 {
        i += 2;
    }

    Assert::eq(i, 10);
}

fn test_basic_while_loop_down(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let i = 10;
    while i > 0 {
        i -= 1;
    }

    Assert::eq(i, 0);
}

fn test_basic_while_loop_up(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let i = 0;
    while i < 10 {
        i += 1;
    }

    Assert::eq(i, 10);
}

# ENTRYPOINT
@pub @world
fn run(should_print, crash_on_failure) {
    let out = should_print;
    Assert::CRASH_ON_FAILURE = crash_on_failure;

    test_basic_while_loop_up(out);
    test_basic_while_loop_down(out);
    test_basic_while_loop_up_wstride(out);
    test_while_loop_big_up(out);
    test_while_loop_big_down(out);
    test_while_loop_break(out);
    test_while_loop_continue(out);
    test_while_loop_count(out);
    test_while_loop_return(out);
}
