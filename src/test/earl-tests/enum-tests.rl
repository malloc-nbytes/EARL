module EnumTests

import "std/assert.rl";
import "test-utils.rl";

Assert::FILE = __FILE__;

enum Test1 {
    I1,
    I2,
    I3,
    I4,
    I5,
}

enum Test2 {
    I1 = 10,
    I2,
    I3,
    I4 = 20,
    I5,
}

enum Test3 {
    I1 = 'a',
    I2 = 'b',
    I3 = "hello",
    I4 = 0..5,
}

fn test_enum_wmultiple_types(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    Assert::eq(Test3.I1, 'a');
    Assert::eq(Test3.I2, 'b');
    Assert::eq(Test3.I3, "hello");
    Assert::eq(Test3.I4, [0,1,2,3,4]);
}

fn test_enum_wassignment(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    Assert::eq(Test2.I1, 10);
    Assert::eq(Test2.I2, 11);
    Assert::eq(Test2.I3, 12);
    Assert::eq(Test2.I4, 20);
    Assert::eq(Test2.I5, 21);
}

fn test_basic_enum(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    Assert::eq(Test1.I1, 0);
    Assert::eq(Test1.I2, 1);
    Assert::eq(Test1.I3, 2);
    Assert::eq(Test1.I4, 3);
    Assert::eq(Test1.I5, 4);
}

# ENTRYPOINT
@pub @world
fn run(should_print, crash_on_failure) {
    let out = should_print;
    Assert::CRASH_ON_FAILURE = crash_on_failure;

    test_basic_enum(out);
    test_enum_wassignment(out);
    test_enum_wmultiple_types(out);
}
