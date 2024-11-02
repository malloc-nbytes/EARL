module ClassTests

import "std/assert.rl";
import "test-utils.rl";
import "imports/import-tests-artifacts.rl";

Assert::FILE = __FILE__;

class TestClass3 [v1, v2] {
    let v1 = v1;
    let v2 = v2;

    fn helper(add) {
        return this.v1 + this.v2 + add;
    }

    @pub fn sum(add) {
        return this.helper(add);
    }
}

class TestClass2 [x, y, z] {
    @pub let x, y, z = (x, y, z);
}

class TestClass1 [] {
    @pub let x = 1;
}

fn test_class_from_other_file(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let p = ImportTestsArtifacts::Point(1, 2);
    Assert::eq(p.psum(), 3);
}

fn test_class_wmethods(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let tc = TestClass3(1, 2);
    Assert::eq(tc.sum(3), 6);
}

fn test_class_wparams(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let tc = TestClass2(1,2,3);
    Assert::eq(tc.x, 1);
    Assert::eq(tc.y, 2);
    Assert::eq(tc.z, 3);
}

fn test_basic_class_instant(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);
    let tc = TestClass1();
    Assert::eq(tc.x, 1);
}

# ENTRYPOINT
@pub @world
fn run(should_print, crash_on_failure) {
    let out = should_print;
    Assert::CRASH_ON_FAILURE = crash_on_failure;

    test_basic_class_instant(out);
    test_class_wparams(out);
    test_class_wmethods(out);
    test_class_from_other_file(out);
}
