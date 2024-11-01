module FloatTests

import "std/assert.rl";
import "test-utils.rl";

Assert::FILE = __FILE__;

fn test_basic_float(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let f0 = 1.0;
    let f1 = 2.;
    let f2 = 0.3;
    let f3 = .6569;

    Assert::eq(f0, 1.0);
    Assert::eq(f1, 2.0);
    Assert::eq(f2, 0.3);
    Assert::eq(f3, 0.6569);
}

# ENTRYPOINT
@pub @world
fn run(should_print, crash_on_failure) {
    let out = should_print;
    Assert::CRASH_ON_FAILURE = crash_on_failure;

    test_basic_float(out);
}
