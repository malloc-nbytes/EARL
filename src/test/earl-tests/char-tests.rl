module CharTests

import "std/assert.rl";
import "test-utils.rl";

Assert::FILE = __FILE__;

fn test_char_ascii_method_intrinsic(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let i = 97;
    foreach c in 'a'..='z' {
        Assert::eq(c.ascii(), i);
        i += 1;
    }
}

fn test_basic_char(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let c = 'a';
    let b = 'b';

    Assert::eq(c, 'a');
    Assert::eq(b, 'b');

    Assert::is_true(c != b);
}

# ENTRYPOINT
@pub @world
fn run(should_print, crash_on_failure) {
    let out = should_print;
    Assert::CRASH_ON_FAILURE = crash_on_failure;

    test_basic_char(out);
    test_char_ascii_method_intrinsic(out);
}
