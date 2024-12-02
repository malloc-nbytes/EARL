module EncryptionModuleTests

import "std/assert.rl";
import "std/algorithms/encryption.rl";
import "test-utils.rl";

Assert::FILE = __FILE__;

fn test_rle(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let s = "abbcccdeefffg";
    let encr = Encryption::rle(s);
    Assert::eq(encr, "a1b2c3d1e2f3g1");
}

# ENTRYPOINT
@pub @world
fn run(should_print, crash_on_failure) {
    let out = should_print;
    Assert::CRASH_ON_FAILURE = crash_on_failure;

    test_rle(out);
}
