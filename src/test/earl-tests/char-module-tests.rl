module CharModuleTests

import "std/assert.rl";
import "std/datatypes/char.rl";
import "test-utils.rl";

Assert::FILE = __FILE__;

fn test_toupper(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    Assert::eq(Char::toupper('a'), 'A');
    Assert::eq(Char::toupper('z'), 'Z');
    Assert::eq(Char::toupper('@'), '@');
}

fn test_tolower(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    Assert::eq(Char::tolower('A'), 'a');
    Assert::eq(Char::tolower('Z'), 'z');
    Assert::eq(Char::tolower('@'), '@');
}

fn test_isalnum(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    Assert::is_true(Char::isalnum('a'));
    Assert::is_true(Char::isalnum('3'));
    Assert::is_true(Char::isalnum('b'));
    Assert::is_true(Char::isalnum('9'));
    Assert::is_false(Char::isalnum('_'));
    Assert::is_false(Char::isalnum('@'));
}

fn test_isnum(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    Assert::is_true(Char::isnum('1'));
    Assert::is_true(Char::isnum('2'));
    Assert::is_true(Char::isnum('3'));
    Assert::is_true(Char::isnum('9'));
    Assert::is_false(Char::isnum('a'));
}

fn test_isalpha(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    Assert::is_true(Char::isalpha('a'));
    Assert::is_true(Char::isalpha('b'));
    Assert::is_true(Char::isalpha('c'));
    Assert::is_true(Char::isalpha('d'));
}

# ENTRYPOINT
@pub @world
fn run(should_print, crash_on_failure) {
    let out = should_print;
    Assert::CRASH_ON_FAILURE = crash_on_failure;

    test_isalpha(out);
    test_isnum(out);
    test_isalnum(out);
    test_tolower(out);
    test_toupper(out);
}
