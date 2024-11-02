module TupleTests

import "std/assert.rl";
import "test-utils.rl";

Assert::FILE = __FILE__;

fn test_tuple_contains(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let t = (1,'a',3,4,'b');

    Assert::is_true(t.contains('a'));
    Assert::is_true(t.contains('b'));
    Assert::is_true(t.contains(4));
    Assert::is_false(t.contains(9));
}

fn test_tuple_foreach(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let lst = [];
    let t = (1,2,3,4,5);
    t.foreach(|x| { lst.append(x); });
    Assert::eq(lst, 1..=5);
}

fn test_tuple_filter(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let t = (1,2,3,4,5,6,7,8,9,10);
    let f = t.filter(|x| { return x%2 == 0; });
    Assert::eq(f, (2,4,6,8,10));

    Assert::eq((1,2,3), (1,2,3,4,5).filter(|x| { return x == 1 || x == 2 || x == 3; }));
}

fn test_tuple_rev(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    Assert::eq((1,2,3).rev(), (3,2,1));

    let t = (1,2,3);
    let t2 = t.rev();
    Assert::eq(t, (1,2,3));
    Assert::eq(t2, (3,2,1));
}

fn test_tuple_destr(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    fn aux() {
        return (1, 2);
    }

    let x, y, z = (1, 2, 3);
    Assert::eq(x, 1);
    Assert::eq(y, 2);
    Assert::eq(z, 3);

    let a, b = aux();
    Assert::eq(a, 1);
    Assert::eq(b, 2);

    let t = ('a', none);
    let k, j = t;
    Assert::eq(k, 'a');
    Assert::eq(j, none);
}

fn test_basic_tuple(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let t = (1,2,3);
    Assert::eq(t, (1,2,3));

    let t2 = (1,);
    Assert::eq(t2, (1,));
}

# ENTRYPOINT
@pub @world
fn run(should_print, crash_on_failure) {
    let out = should_print;
    Assert::CRASH_ON_FAILURE = crash_on_failure;

    test_basic_tuple(out);
    test_tuple_destr(out);
    test_tuple_rev(out);
    test_tuple_filter(out);
    test_tuple_foreach(out);
    test_tuple_contains(out);
}
