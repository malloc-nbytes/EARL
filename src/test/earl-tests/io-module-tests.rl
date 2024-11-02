module IOModuleTests

import "std/assert.rl";
import "std/io.rl";
import "test-utils.rl";

Assert::FILE = __FILE__;

fn test_read_lines(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let fp = "./other-files/read-lines-test.txt";
    let s = IO::read_lines(fp);
    Assert::eq(s, ["foo", "bar", "baz", ""]);
}

fn test_file_to_str(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let fp = "./other-files/file-to-str-test.txt";
    let s = IO::file_to_str(fp);
    Assert::eq(s, "hello world\n");
}

fn test_Fd(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    Assert::eq(IO::Fd.In, 0);
    Assert::eq(IO::Fd.Out, 1);
    Assert::eq(IO::Fd.Err, 2);
}

# ENTRYPOINT
@pub @world
fn run(should_print, crash_on_failure) {
    let out = should_print;
    Assert::CRASH_ON_FAILURE = crash_on_failure;

    test_Fd(out);
    test_file_to_str(out);
    test_read_lines(out);
}
