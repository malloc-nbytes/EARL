module IntrinsicsTests

import "std/assert.rl";
import "test-utils.rl";

Assert::FILE = __FILE__;

fn test_intrinsic_dict(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);
    # todo
}

fn test_intrinsic_unit(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);
    # todo
}

fn test_intrinsic_list(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);
    # todo
}

fn test_intrinsic_tuple(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    Assert::eq(tuple(1, 2), (1, 2));
}

fn test_intrinsic_bool(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let b = bool("false");
    Assert::eq(typeof(b), bool);
    Assert::is_false(b);
}

fn test_intrinsic_float(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let f = float(3);
    Assert::eq(typeof(f), float);
    Assert::eq(f, 3.0);
}

fn test_intrinsic_int(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let x = int(1.2);
    Assert::eq(typeof(x), int);
    Assert::eq(x, 1);
}

fn test_intrinsic_str(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    Assert::eq(str(1), "1");
    Assert::eq(str([1, 2, 3]), "[1, 2, 3]");
    Assert::eq(str(none), "none");
    Assert::eq(str(true), "true");
}

fn test_intrinsic_typeof(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    Assert::eq(typeof(1), int);
    Assert::eq(typeof(1.), float);
    Assert::eq(typeof("foo"), str);
}

fn test_intrinsic_type(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    Assert::eq(type(1), "int");
    Assert::eq(type("hello"), "str");
    Assert::eq(type(none), "option");
    Assert::eq(type(1..5), "list");
}

fn test_intrinsic_open(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let f = open("other-files/extra.txt", "r");
    let s = f.read();
    Assert::eq(s, f.read());
    f.close();
}

fn test_intrinsic_len(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let lst = 0..100;
    Assert::eq(len(lst), 100);
    Assert::eq(len(0..100), 100);
    Assert::eq(len("hello"), 5);
    Assert::eq(len((1,2,3,4,5)), 5);
}

fn test_intrinsic_assert(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    assert(1 == 1);
    assert(int != float);
    assert(len("hi") == 2);
}


# ENTRYPOINT
@pub @world
fn run(should_print, crash_on_failure) {
    let out = should_print;
    Assert::CRASH_ON_FAILURE = crash_on_failure;

    test_intrinsic_len(out);
    test_intrinsic_assert(out);
    test_intrinsic_len(out);
    test_intrinsic_open(out);
    test_intrinsic_type(out);
    test_intrinsic_typeof(out);
    test_intrinsic_str(out);
    test_intrinsic_int(out);
    test_intrinsic_float(out);
    test_intrinsic_bool(out);
    test_intrinsic_tuple(out);
    test_intrinsic_list(out);
    test_intrinsic_unit(out);
    test_intrinsic_dict(out);
}
