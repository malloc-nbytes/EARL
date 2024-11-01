module IfTests

import "std/assert.rl";
import "test-utils.rl";

Assert::FILE = __FILE__;

fn test_basic_if_else_if(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let i = 0;

    if i == 5 {
        Assert::is_true(false);
    }
    else if i == 3 {
        Assert::is_true(false);
    }
    else if i == 0 {
        i = 9;
    }
    else {
        Assert::is_true(false);
    }

    Assert::eq(i, 9);
}

fn test_basic_if_else(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let i = 0;

    if i == 1 {
        Assert::is_true(false);
    }
    else {
        i = 9;
    }

    Assert::eq(i, 9);
}

fn test_basic_if(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let i = 10;

    if i == 10 {
        i = 99;
    }

    if i == 90000 {
        i = 50;
    }

    Assert::eq(i, 99);
}

# ENTRYPOINT
@pub @world
fn run(should_print, crash_on_failure) {
    let out = should_print;
    Assert::CRASH_ON_FAILURE = crash_on_failure;

    test_basic_if(out);
    test_basic_if_else(out);
    test_basic_if_else_if(out);
}
