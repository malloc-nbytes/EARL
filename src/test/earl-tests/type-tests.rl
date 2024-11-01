module TypeTests

import "std/assert.rl";
import "test-utils.rl";
import "imports/import-tests-artifacts.rl";

Assert::FILE = __FILE__;

class TestTypeClass [x: int, y: str] {
    @pub let x: int = x;
    @pub let y: str = y;

    @pub fn do_something(z: int): tuple {
        return (z, this.x, this.y);
    }
}

fn test_external_class_type(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let p: ImportTestsArtifacts::Point = ImportTestsArtifacts::Point(1, 2);
    let p2: Point = ImportTestsArtifacts::Point(3, 4);

    Assert::eq(p.psum(), 3);
    Assert::eq(p2.psum(), 7);
}

fn test_class_type(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let t: TestTypeClass = TestTypeClass(1, "bar");
    Assert::eq(t.do_something(3), (3, 1, "bar"));
}

fn test_fn_types(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    fn aux1(): int {
        return 1;
    }

    fn aux2(x: int, y: int): int {
        return x+y;
    }

    Assert::eq(aux1(), 1);
    Assert::eq(aux2(1, 2), 3);
}

fn test_basic_vartypes(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let x: int = 1;
    let y: int = 2;

    let s: str, i: type = ("foo", option);
}

# ENTRYPOINT
@pub @world
fn run(should_print, crash_on_failure) {
    let out = should_print;
    Assert::CRASH_ON_FAILURE = crash_on_failure;

    test_basic_vartypes(out);
    test_fn_types(out);
    test_class_type(out);
    test_external_class_type(out);
}
