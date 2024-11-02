module FnTests

import "std/assert.rl";
import "test-utils.rl";

Assert::FILE = __FILE__;

fn test_fn_inside_closure(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let cl = |_| {
        fn aux() {
            return 1;
        }
        return aux();
    };

    Assert::eq(cl(), 1);
}

fn test_recursion_wno_return_value(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    fn aux(@ref i) {
        if i == 100 {
            return;
        }
        i += 1;
        aux(i);
    }

    let i = 0;
    aux(i);
    Assert::eq(i, 100);
}

fn test_recursion_wreturn_value(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    fn aux(i) {
        if i == 100 {
            return i;
        }
        return aux(i+1);
    }

    Assert::eq(aux(0), 100);
}

fn test_nested_fns(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    fn aux() {
        fn inner() {
            return 9;
        }
        return inner();
    }

    Assert::eq(aux(), 9);
}

# ENTRYPOINT
@pub @world
fn run(should_print, crash_on_failure) {
    let out = should_print;
    Assert::CRASH_ON_FAILURE = crash_on_failure;

    test_nested_fns(out);
    test_recursion_wreturn_value(out);
    test_recursion_wno_return_value(out);
    test_fn_inside_closure(out);
}
