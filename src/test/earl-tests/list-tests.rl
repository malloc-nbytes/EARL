module ListTests

import "std/assert.rl";
import "test-utils.rl";

Assert::FILE = __FILE__;

fn test_list_contains(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let lst = 'a'..='z';
    Assert::is_true(lst.contains('a'));
    Assert::is_true(lst.contains('d'));
    Assert::is_true(lst.contains('z'));
    Assert::is_false(lst.contains('?'));
}

fn test_list_map(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let m = (0..=1000).map(|x| { return x+2; });
    Assert::eq(m, 2..=1002);
}

fn test_list_foreach(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let lst = 0..=10000;
    let c = 0;
    lst.foreach(|@const x| {
        Assert::eq(x, c);
        c += 1;
    });

    lst.foreach(|@ref x| {
        x = 9;
    });

    lst.foreach(|x| { Assert::eq(x, 9); });
}

fn test_list_filter(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let lst = (0..=10).filter(|k| { return k%2 == 0; });

    Assert::eq(lst, [0,2,4,6,8,10]);
}

fn test_list_rev(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let lst = [1,2,3,4,5];
    lst = lst.rev();
    Assert::eq(lst, [5,4,3,2,1]);
}

fn test_list_pop(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let lst = 0..10;
    lst.pop(0);
    lst.pop(len(lst)-1);

    Assert::eq(lst, [1,2,3,4,5,6,7,8]);
}

fn test_list_append(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let lst = [];
    for i in 0 to 10 {
        lst.append(i);
    }

    Assert::eq(len(lst), 10);
}

fn test_list_add(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let lst = [1,2];
    lst += [3,4] + [5,6];
    Assert::eq(lst, 1..=6);
}

fn test_basic_list(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let lst = [1,2,3,4,5];

    Assert::eq(lst, [1,2,3,4,5]);
    Assert::eq(lst, 1..=5);

    let lst2 = [1,2,'a'];

    # Assert::is_true(lst != lst2);
}

# ENTRYPOINT
@pub @world
fn run(should_print, crash_on_failure) {
    let out = should_print;
    Assert::CRASH_ON_FAILURE = crash_on_failure;

    test_basic_list(out);
    test_list_add(out);
    test_list_append(out);
    test_list_pop(out);
    test_list_rev(out);
    test_list_filter(out);
    test_list_foreach(out);
    test_list_map(out);
    test_list_contains(out);
}
