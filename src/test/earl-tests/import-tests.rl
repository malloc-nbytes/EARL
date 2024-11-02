module ImportTests

import "std/assert.rl";
import "test-utils.rl";

import "imports/import-tests-artifacts.rl";

Assert::FILE = __FILE__;

fn test_import_enum(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    Assert::eq(ImportTestsArtifacts::ExternalEnum.I1, 0);
    Assert::eq(ImportTestsArtifacts::ExternalEnum.I2, 1);
    Assert::eq(ImportTestsArtifacts::ExternalEnum.I3, 2);
}

fn test_import_class(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    let p = ImportTestsArtifacts::Point(3, 4);
    Assert::eq(p.x, 3);
    Assert::eq(p.y, 4);
}

fn test_import_fns(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    Assert::eq(ImportTestsArtifacts::sum(1, 2), 3);
}

fn test_import_vars(out) {
    TestUtils::log(out, __FILE__, __FUNC__, Assert::FUNC);

    Assert::eq(ImportTestsArtifacts::X, 1);
    Assert::eq(ImportTestsArtifacts::Y, 2);
}

# ENTRYPOINT
@pub @world
fn run(should_print, crash_on_failure) {
    let out = should_print;
    Assert::CRASH_ON_FAILURE = crash_on_failure;

    test_import_vars(out);
    test_import_fns(out);
    test_import_class(out);
}
