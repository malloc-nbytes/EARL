module IntTests

import "std/assert.rl";
import "test-utils.rl";

Assert::FILE = __FILE__;

fn test_basic_int(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let i = 0;
    Assert::eq(i, 0);

    i = 9;
    Assert::eq(i, 9);

    i -= 1;
    Assert::eq(i, 8);

    i += 2;
    Assert::eq(i, 10);

    i /= 2;
    Assert::eq(i, 5);

    let b = 10;
    i += b;
    Assert::eq(i, 15);
}

# ENTRYPOINT
@pub @world
fn run(should_print, crash_on_failure) {
    let out = should_print;
    Assert::CRASH_ON_FAILURE = crash_on_failure;

    test_basic_int(out);
}
