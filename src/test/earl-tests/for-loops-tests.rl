module ForLoopTests

import "test-utils.rl";
import "std/assert.rl";

Assert::FILE = __FILE__;

fn test_for_loop_break(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let c = 0;
    for i in 0 to 10000 {
        if i == 10000/2 {
            break;
        }
        c += 1;
    }

    Assert::eq(c, 10000/2);
}

fn test_for_loop_continue(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let c = 0;
    for i in 0 to 100 {
        if i%2 == 0 {
            continue;
        }
        c += 1;
    }

    Assert::eq(c, 50);
}

fn test_for_loop_wstride(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let c = 0;
    for i in 0 to 100 {
        c += 1;
        i += 1;
    }

    Assert::eq(c, 50);
}

fn test_for_loop_down_big(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let c, cap = (0, 500000);
    for i in cap to 0 {
        c += 1;
    }

    Assert::eq(c, cap+1);
}

fn test_for_loop_up_big(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let c, cap = (0, 500000);
    for i in 0 to cap {
        c += 1;
    }

    Assert::eq(c, cap);
}

fn test_basic_for_loop_down(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let c = 10;
    for i in 10 to 0 {
        Assert::eq(i, c);
        c -= 1;
    }
}

fn test_basic_for_loop_up(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);
    let c = 0;
    for i in 0 to 10 {
        Assert::eq(c, i);
        c += 1;
    }
}

# ENTRYPOINT
@pub @world
fn run(should_print, crash_on_failure) {
    let out = should_print;
    Assert::CRASH_ON_FAILURE = crash_on_failure;
    test_basic_for_loop_up(out);
    test_basic_for_loop_down(out);
    test_for_loop_up_big(out);
    test_for_loop_down_big(out);
    test_for_loop_wstride(out);
    test_for_loop_continue(out);
    test_for_loop_break(out);
}
