# Change Log
All notable changes to this project will be documented in this file.

## [0.3.9] - 2024-25-8

### Added
- contains() member intrinsic for lists, tuples, and strs.

### Fixed
- You can now use the 'this' keyword in closures inside of classes.

## [0.3.8] - 2024-24-8

### Added
- Power operator `**`.

### Fixed
- Fixed segfaulting on parser errors when hot reloading.
- Fixed the len() unreachable case happening.

### Changed
- All EARL values now have an appropriate to_cxxstring.
- Greatly simplified __intrinsic_print.
- Better closure printing.
- Intrinsics now report better error messages.

## [0.3.6] - 2024-23-8

### Added
- Fstrings.

### Fixed
- Fixed the bug where foreach loops would always take the value that they are iterating over as a reference.

## [0.3.5] - 2024-22-8

### Added
- Bitwise operators.

## [0.3.4] - 2024-21-8

### Fixed
- Using .append() on a list no longer always does an unnecessary copy.
- Updated class deep copy to now actually copy over methods.
- Fixed segfault with incorrect function/closure paremeters are provided in evaluate\_function\_parameters\_wrefs.
- Fixed segfault because function\_exists and function\_get for function context not being the same.

## [0.3.3] - 2024-20-8

### Added
- New flag to watch files for changes and perform hot reloading.
- String optimization
- Implemented the `continue` keyword.

### Fixed
- List/str/dictionary/tuple indexing no longer returns the value as a reference by default.
- Strings can now have delimiters again.
- Fixed the bug where dictionaries would end parsing expressions.

### Changed
- Dictionaries now return none if the value does not exist, and some(V) if it does exist.

## [0.3.1] - 2024-19-8

### Added
- Some recursion optimizations

### Fixed
- Calling a class method that has the same name as a member intrinsic no longer fails.

### Changed
- std/set.earl now uses an AVL tree instead of a regulary binary search tree.

## [0.3.0] - 2024-18-8

### Added
- MAJOR Added new Dictionary type.
- Added new TypeKW type.

### Changed
- Unit types now return false in boolean expressions.

## [0.2.6] - 2024-17-8

### Added
- Added new Char module to stdlib
- Added tuple destructuring to allow for multiple variables to be declared in a single `let` statement.

### Fixed
- Classes allowing duplicate member variables.
- Precedence changes to ranges and slices.
- MAJOR Fixed nested for loop bug where the second enumerator would point to the first if set to it.

## [0.2.4] - 2024-14-8

### Added
- Added list slicing
- [MAJOR] Added much better error reporting during runtime (interpreter.cpp).

### Fixed
- Better error messages for the expect_keyword function in parser.
- The minimum cmake version in the README is now up to date with what it actually it.

## [0.2.2] - 2024-13-8

### Added
- Ranges can now be inclusive using the `=` symbol.

### Fixes
- Fixed the bug when removing or editing a line of code in the REPL that had a bracket.

### Changed
- MAJOR New build system CMake
- Adjusted minimum CMake version to support Debian

## [0.2.1] - 2024-12-8

### Added
- New name_and_ext function in the OS module.

### Fixes
- Performing str += str caused a segfault because of `char` conversion.
- Fixed having excess semicolons segfaulting. They now throw an error.
- MAJOR Fixed array null access that was causing segfaults on MacOS.
- Fixed allowing the creation of duplicate enums of the same name.
- Fixed segfault with invalid getter operation. It now throws an error.

## [0.2.0] - 2024-11-8

### Added
- Added Intrinsic casting functions.
- MAJOR Added a REPL

### Fixed
- Fixed file handler mode type to except multiple types
- The appropriate unit functions now return a unit type instead of null.

## [0.1.3] - 2024-10-8

### Added
- MAJOR New Tuple type

## [0.1.2] - 2024-09-8

### Changes
- MAJOR `foreach` loops now take an expression (list/range) as the argument.
- MAJOR in `foreach` loops, the enumerator can now have attributes.
- MAJOR `<list>.rev()` does not reverse the list in-place anymore. It now produces a new list with the elements reversed.
- MAJOR changed syntax for the regular `for` loops.

### Added
- Syntax sugar for lists in the form of a range ie [0..10] (range is from 0-9) or ['a'..'z'].
- Member variables and methods in classes now adhere to the @pub and private system

## [0.1.1] - 2024-08-8

### Changes
- Renamed keyword `mod` -> `module`

### Added
- Enums
- Float type
- Unary Expressions
- Better output when printing enumerations
- Import depth
- Enum type security
- Better error messages for type mismatchs

### Fixed

- MINOR Fixed segfault bug when creating a float without a left number ie `.34`.
- MAJOR Fixed integer+float bug
