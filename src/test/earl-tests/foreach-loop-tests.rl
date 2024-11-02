module ForeachLoopTests

import "std/assert.rl";
import "test-utils.rl";

Assert::FILE = __FILE__;

fn test_foreach_dict(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let dict = {
        'a': none,
        'b': 1,
        'c': 0..5
    };

    let shouldbe = [
        ('a', none),
        ('b', 1),
        ('c', 0..5),
    ];

    let c = 0;
    foreach key, value in dict {
        Assert::eq(key, shouldbe[c][0]);
        Assert::eq(value, shouldbe[c][1]);
        c += 1;
    }
}

fn test_foreach_tuple(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let lst = [(1,2,3), (4,5,6), (7,8,9)];
    let shouldbe = [1,2,3];
    let c = 0;
    foreach i, j, k in lst {
        Assert::eq(i, shouldbe[0]);
        Assert::eq(j, shouldbe[1]);
        Assert::eq(k, shouldbe[2]);

        shouldbe[0] += 3;
        shouldbe[1] += 3;
        shouldbe[2] += 3;
    }
}

fn test_foreach_ref(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let lst = 0..100;
    foreach @ref i in lst {
        i = 9;
    }
    for i in 0 to len(lst) {
        Assert::eq(lst[i], 9);
    }

    Assert::eq(len(lst), 100);
}

fn test_foreach_continue(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let c = 0;
    foreach i in 0..100 {
        if i%2 == 0 {
            continue;
        }
        c += 1;
    }

    Assert::eq(c, 100/2);
}

fn test_foreach_break(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let c = 0;
    foreach i in 0..200 {
        if i == 100 {
            break;
        }
        c += 1;
    }

    Assert::eq(c, 200/2);
}

fn test_foreach_over_lst(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let lst = 0..1000;
    let i = 0;
    foreach el in lst {
        Assert::eq(el, lst[i]);
        i += 1;
    }
}

fn test_basic_foreach_up_big(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let c, cap = (0, 10000);
    foreach i in 0..cap {
        Assert::eq(i, c);
        c += 1;
    }
}

fn test_basic_foreach_down(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let c = 10;
    foreach i in (0..=10).rev() {
        Assert::eq(i, c);
        c -= 1;
    }
}

fn test_basic_foreach_up(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let c = 0;
    foreach i in 0..10 {
        Assert::eq(i, c);
        c += 1;
    }

    Assert::eq(c, 10);
}

# ENTRYPOINT
@pub @world
fn run(should_print, crash_on_failure) {
    let out = should_print;
    Assert::CRASH_ON_FAILURE = crash_on_failure;

    test_basic_foreach_up(out);
    test_basic_foreach_down(out);
    test_basic_foreach_up_big(out);
    test_foreach_over_lst(out);
    test_foreach_break(out);
    test_foreach_continue(out);
    test_foreach_ref(out);
    test_foreach_tuple(out);
    test_foreach_dict(out);
}
