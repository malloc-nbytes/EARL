module OptionTests

import "std/assert.rl";
import "test-utils.rl";

Assert::FILE = __FILE__;

fn test_option_unwrap(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let o = some(1);
    Assert::eq(o.unwrap(), 1);
    o = some(1..5);
    Assert::eq(o.unwrap(), 1..5);
}

fn test_option_is_some(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let o = some(1);
    Assert::is_true(o.is_some());
    o = none;
    o = some('a');
    Assert::is_true(o.is_some());
}

fn test_option_is_none(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let o = none;
    Assert::is_true(o.is_none());
    o = some(1);
    o = none;
    Assert::is_true(o.is_none());
}

fn test_basic_option(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let o = none;
    Assert::is_none(o);

    o = some(1);
    Assert::not_none(o);
    Assert::eq(o, some(1));

    o = some('a');
    Assert::not_none(o);
    Assert::eq(o, some('a'));
}

# ENTRYPOINT
@pub @world
fn run(should_print, crash_on_failure) {
    let out = should_print;
    Assert::CRASH_ON_FAILURE = crash_on_failure;

    test_basic_option(out);
    test_option_is_none(out);
    test_option_is_some(out);
    test_option_unwrap(out);
}
